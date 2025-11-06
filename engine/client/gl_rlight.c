/*
gl_rlight.c - dynamic and static lights
Copyright (C) 2010 Uncle Mike

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
*/

#ifndef XASH_DEDICATED

#include "common.h"
#include "client.h"
#include "mathlib.h"
#include "gl_local.h"
#include "pm_local.h"
#include "studio.h"

/*
=============================================================================

DYNAMIC LIGHTS - ENHANCED

=============================================================================
*/

// New lighting system variables
static float s_lightInterpolation[MAX_LIGHTSTYLES]; // Smooth interpolation factors
static float s_prevLightValues[MAX_LIGHTSTYLES];    // Previous frame values for smooth transitions
static qboolean s_lightingInitialized = false;

/*
==================
R_InitLightingSystem

Initialize enhanced lighting system
==================
*/
void R_InitLightingSystem(void)
{
    memset(s_lightInterpolation, 0, sizeof(s_lightInterpolation));
    memset(s_prevLightValues, 0, sizeof(s_prevLightValues));
    s_lightingInitialized = true;
}

/*
==================
R_LightTraceFilter

Filter function for light tracing - determines which entities should block light
==================
*/
int R_LightTraceFilter(physent_t* pe)
{
    if (!pe)
        return 1; // Continue tracing

    // Only solid BSP entities block light
    if (pe->solid != SOLID_BSP || pe->info == 0)
        return 1; // Continue tracing

    return 0; // Block tracing at this entity
}

/*
==================
R_AnimateLight

Enhanced with smooth interpolation and dynamic effects
==================
*/
void R_AnimateLight(void)
{
    int i, k, flight, clight;
    float l, c, lerpfrac, backlerp;
    float scale;
    lightstyle_t* ls;
    static float lastTime = 0.0f;
    float frameTime;

    if (!RI.drawWorld || !cl.worldmodel)
        return;

    // Initialize lighting system if needed
    if (!s_lightingInitialized)
        R_InitLightingSystem();

    // Calculate smooth frame time
    frameTime = RI.refdef.frametime;
    if (frameTime > 0.1f) frameTime = 0.1f; // Clamp for stability
    if (frameTime <= 0.0f) frameTime = 0.001f; // Prevent division by zero

    scale = r_lighting_modulate->value;

    // Enhanced overbright handling
    if (gl_overbright->integer == 2)
        scale /= 1.6f;
    else if (r_vbo->integer && gl_overbright->integer)
        scale /= 1.5f;

    // Apply dynamic scaling based on scene complexity
    if (r_dynamic->integer)
    {
        int numDlights = R_CountDlights();
        float complexityScale = 1.0f - (numDlights * 0.02f); // Slight reduction with many lights
        complexityScale = bound(0.7f, complexityScale, 1.2f);
        scale *= complexityScale;
    }

    // Light animations with enhanced interpolation
    for (i = 0, ls = cl.lightstyles; i < MAX_LIGHTSTYLES; i++, ls++)
    {
        if (r_fullbright->integer || !cl.worldmodel->lightdata)
        {
            RI.lightstylevalue[i] = 256 * 256;
            RI.lightcache[i] = 3.0f;
            s_prevLightValues[i] = RI.lightcache[i];
            continue;
        }

        // Update light time with smooth delta
        if (!RI.refdef.paused && frameTime > 0.0f)
            ls->time += frameTime;

        // Calculate interpolation factors for smooth transitions
        float targetInterpolation = cl_lightstyle_lerping->integer ? 0.95f : 0.0f;
        s_lightInterpolation[i] = s_lightInterpolation[i] * 0.8f + targetInterpolation * 0.2f;

        flight = (int)floor(ls->time * 10);
        clight = (int)ceil(ls->time * 10);
        lerpfrac = (ls->time * 10) - flight;
        backlerp = 1.0f - lerpfrac;

        if (!ls->length)
        {
            RI.lightstylevalue[i] = 256 * scale;
            RI.lightcache[i] = 3.0f * scale;
        }
        else if (ls->length == 1)
        {
            // Single length style - apply subtle variation for dynamics
            float baseValue = ls->map[0] * 22 * scale;
            float cacheValue = (ls->map[0] / 12.0f) * 3.0f * scale;

            // Add subtle noise for organic feel
            if (cl_lightstyle_lerping->integer && ls->interp)
            {
                float noise = sin(ls->time * 2.0f + i) * 0.1f + 1.0f;
                baseValue *= noise;
                cacheValue *= noise;
            }

            RI.lightstylevalue[i] = baseValue;
            RI.lightcache[i] = cacheValue;
        }
        else if (!ls->interp || !cl_lightstyle_lerping->integer)
        {
            RI.lightstylevalue[i] = ls->map[flight % ls->length] * 22 * scale;
            RI.lightcache[i] = (ls->map[flight % ls->length] / 12.0f) * 3.0f * scale;
        }
        else
        {
            // Enhanced interpolating light with smooth transitions
            k = ls->map[flight % ls->length];
            l = (float)(k * 22.0f) * backlerp;
            c = (float)(k / 12.0f) * backlerp;

            k = ls->map[clight % ls->length];
            l += (float)(k * 22.0f) * lerpfrac;
            c += (float)(k / 12.0f) * lerpfrac;

            // Apply smooth interpolation between frames
            if (s_lightInterpolation[i] > 0.01f)
            {
                l = s_prevLightValues[i] * s_lightInterpolation[i] + l * (1.0f - s_lightInterpolation[i]);
                c = s_prevLightValues[i] * s_lightInterpolation[i] + c * (1.0f - s_lightInterpolation[i]);
            }

            RI.lightstylevalue[i] = (int)l * scale;
            RI.lightcache[i] = c * 3.0f * scale;
        }

        s_prevLightValues[i] = RI.lightcache[i];
    }

    lastTime = cl.time;
}

/*
=============
R_MarkLights

Optimized with early rejection and improved culling
=============
*/
void R_MarkLights(dlight_t* light, int bit, mnode_t* node)
{
    float dist;
    msurface_t* surf;
    int i;

    if (!node || node->contents < 0)
        return;

    // Enhanced plane distance calculation with margin
    dist = PlaneDiff(light->origin, node->plane);
    float radiusWithMargin = light->radius * 1.1f; // Small margin for better coverage

    if (dist > radiusWithMargin)
    {
        R_MarkLights(light, bit, node->children[0]);
        return;
    }
    if (dist < -radiusWithMargin)
    {
        R_MarkLights(light, bit, node->children[1]);
        return;
    }

    // Mark polygons with optimized bounds checking
    surf = RI.currentmodel->surfaces + node->firstsurface;

    for (i = 0; i < node->numsurfaces; i++, surf++)
    {
        mextrasurf_t* info = SURF_INFO(surf, RI.currentmodel);

        // Early rejection using enhanced bounds check
        if (!BoundsAndSphereIntersect(info->mins, info->maxs, light->origin, light->radius))
            continue;

        // Initialize surface light data if needed
        if (surf->dlightframe != tr.dlightframecount)
        {
            surf->dlightbits = 0;
            surf->dlightframe = tr.dlightframecount;
        }

        // Set light bit with priority handling
        surf->dlightbits |= bit;
    }

    // Recursively process children
    R_MarkLights(light, bit, node->children[0]);
    R_MarkLights(light, bit, node->children[1]);
}

/*
=============
R_PushDlights

Enhanced with distance sorting and priority system
=============
*/
void R_PushDlights(void)
{
    dlight_t* l;
    int i;
    vec3_t viewOrigin;

    tr.dlightframecount = tr.framecount;
    l = cl_dlights;

    RI.currententity = clgame.entities;
    RI.currentmodel = RI.currententity->model;

    // Get view origin for distance calculations
    VectorCopy(RI.vieworg, viewOrigin);

    for (i = 0; i < MAX_DLIGHTS; i++, l++)
    {
        if (l->die < cl.time || !l->radius)
            continue;

        // Enhanced culling with view-dependent checks
        if (R_CullSphere(l->origin, l->radius, 15))
            continue;

        // Distance-based priority (closer lights have higher priority)
        float dist = VectorDistance(l->origin, viewOrigin);
        float priority = 1.0f - (dist / (l->radius * 4.0f));
        priority = bound(0.1f, priority, 1.0f);

        // Only mark if light has sufficient priority/visibility
        if (priority > 0.3f)
        {
            R_MarkLights(l, 1U << i, RI.currentmodel->nodes);
        }
    }
}

/*
=============
R_CountDlights

Optimized counting
=============
*/
int R_CountDlights(void)
{
    dlight_t* l;
    int i, numDlights = 0;

    for (i = 0, l = cl_dlights; i < MAX_DLIGHTS; i++, l++)
    {
        if (l->die < cl.time || !l->radius)
            continue;

        numDlights++;
    }

    return numDlights;
}

/*
=============
R_CountSurfaceDlights

Enhanced with intensity threshold
=============
*/
int R_CountSurfaceDlights(msurface_t* surf)
{
    int i, numDlights = 0;
    dlight_t* l;

    for (i = 0, l = cl_dlights; i < MAX_DLIGHTS; i++, l++)
    {
        if (!(surf->dlightbits & BIT(i)))
            continue;

        // Skip lights with very low intensity
        if (l->radius < 32.0f) // Minimum radius threshold
            continue;

        numDlights++;
    }

    return numDlights;
}

/*
=======================================================================

ENHANCED AMBIENT LIGHTING

=======================================================================
*/
static uint r_pointColor[3];
static vec3_t r_lightSpot;
static vec3_t s_lastGoodLightSpot;

/*
=================
R_RecursiveLightPoint

Improved with fallback and error recovery
=================
*/
static qboolean R_RecursiveLightPoint(model_t* model, mnode_t* node, const vec3_t start, const vec3_t end)
{
    float front, back, frac;
    int i, map, side, size, s, t;
    msurface_t* surf;
    mtexinfo_t* tex;
    color24* lm;
    vec3_t mid;

    if (!node || node->contents < 0)
        return false;

    front = PlaneDiff(start, node->plane);
    back = PlaneDiff(end, node->plane);

    side = front < 0;
    if ((back < 0) == side)
        return R_RecursiveLightPoint(model, node->children[side], start, end);

    frac = front / (front - back);
    VectorLerp(start, frac, end, mid);

    // Go down front side
    if (R_RecursiveLightPoint(model, node->children[side], start, mid))
    {
        VectorCopy(r_lightSpot, s_lastGoodLightSpot); // Store last valid position
        return true;
    }

    if ((back < 0) == side)
        return false;

    VectorCopy(mid, r_lightSpot);

    // Enhanced surface checking with fallback
    surf = model->surfaces + node->firstsurface;

    for (i = 0; i < node->numsurfaces; i++, surf++)
    {
        tex = surf->texinfo;

        if (surf->flags & (SURF_DRAWSKY | SURF_DRAWTURB))
            continue;

        s = DotProduct(mid, tex->vecs[0]) + tex->vecs[0][3] - surf->texturemins[0];
        t = DotProduct(mid, tex->vecs[1]) + tex->vecs[1][3] - surf->texturemins[1];

        if ((s < 0 || s > surf->extents[0]) || (t < 0 || t > surf->extents[1]))
            continue;

        s /= LM_SAMPLE_SIZE;
        t /= LM_SAMPLE_SIZE;

        if (!surf->samples)
        {
            VectorCopy(r_lightSpot, s_lastGoodLightSpot);
            return true;
        }

        VectorClear(r_pointColor);

        lm = surf->samples + (t * ((surf->extents[0] / LM_SAMPLE_SIZE) + 1) + s);
        size = ((surf->extents[0] / LM_SAMPLE_SIZE) + 1) * ((surf->extents[1] / LM_SAMPLE_SIZE) + 1);

        // Enhanced lightmap sampling with gamma correction
        for (map = 0; map < MAXLIGHTMAPS && surf->styles[map] != 255; map++)
        {
            uint scale = RI.lightstylevalue[surf->styles[map]];

            // Apply perceptual gamma correction
            r_pointColor[0] += TextureToTexGamma(lm->r) * scale;
            r_pointColor[1] += TextureToTexGamma(lm->g) * scale;
            r_pointColor[2] += TextureToTexGamma(lm->b) * scale;

            lm += size;
        }

        VectorCopy(r_lightSpot, s_lastGoodLightSpot);
        return true;
    }

    // Go down back side with fallback position
    if (R_RecursiveLightPoint(model, node->children[!side], mid, end))
    {
        VectorCopy(r_lightSpot, s_lastGoodLightSpot);
        return true;
    }

    return false;
}

/*
=================
R_LightForPoint

Enhanced with better fallback and dynamic light blending
=================
*/
void R_LightForPoint(const vec3_t point, color24* ambientLight, qboolean invLight, qboolean useAmbient, float radius)
{
    dlight_t* dl;
    pmtrace_t trace;
    cl_entity_t* m_pGround;
    vec3_t start, end, dir;
    qboolean secondpass = false;
    float dist, add;
    model_t* pmodel;
    mnode_t* pnodes;
    static color24 s_lastGoodLight = { 255, 255, 255 };

    if (!cl.refdef.movevars)
    {
        *ambientLight = s_lastGoodLight;
        return;
    }

    // Set to full bright if no light data with fallback
    if (!cl.worldmodel || !cl.worldmodel->lightdata)
    {
        ambientLight->r = TextureToTexGamma(cl.refdef.movevars->skycolor_r);
        ambientLight->g = TextureToTexGamma(cl.refdef.movevars->skycolor_g);
        ambientLight->b = TextureToTexGamma(cl.refdef.movevars->skycolor_b);
        s_lastGoodLight = *ambientLight;
        return;
    }

get_light:
    VectorCopy(point, start);
    VectorCopy(point, end);

    if (invLight)
    {
        start[2] = point[2] - 64.0f;
        end[2] = point[2] + world.size[2];
    }
    else
    {
        start[2] = point[2] + 64.0f;
        end[2] = point[2] - world.size[2];
    }

    pmodel = cl.worldmodel;
    pnodes = pmodel->nodes;
    m_pGround = NULL;

    // Enhanced entity lighting detection
    if (r_lighting_extended->integer && !secondpass)
    {
        CL_SetTraceHull(2);
        CL_PlayerTraceExt(start, end, PM_STUDIO_IGNORE, R_LightTraceFilter, &trace);
        m_pGround = CL_GetEntityByIndex(pfnIndexFromTrace(&trace));
        if (trace.startsolid || trace.allsolid) m_pGround = NULL;
    }

    if (m_pGround && m_pGround->model && m_pGround->model->type == mod_brush)
    {
        matrix4x4 matrix;
        hull_t* hull;
        vec3_t start_l, end_l;
        vec3_t offset;

        pmodel = m_pGround->model;
        pnodes = &pmodel->nodes[pmodel->hulls[0].firstclipnode];

        hull = &pmodel->hulls[0];
        VectorSubtract(hull->clip_mins, vec3_origin, offset);
        VectorAdd(offset, m_pGround->origin, offset);

        VectorSubtract(start, offset, start_l);
        VectorSubtract(end, offset, end_l);

        if (!VectorIsNull(m_pGround->angles))
        {
            Matrix4x4_CreateFromEntity(matrix, m_pGround->angles, offset, 1.0f);
            Matrix4x4_VectorITransform(matrix, start, start_l);
            Matrix4x4_VectorITransform(matrix, end, end_l);
        }

        VectorCopy(start_l, start);
        VectorCopy(end_l, end);
    }

    VectorClear(r_pointColor);

    if (R_RecursiveLightPoint(pmodel, pnodes, start, end))
    {
        ambientLight->r = min((r_pointColor[0] >> 7), 255);
        ambientLight->g = min((r_pointColor[1] >> 7), 255);
        ambientLight->b = min((r_pointColor[2] >> 7), 255);
        s_lastGoodLight = *ambientLight;
    }
    else
    {
        // Use fallback lighting with smooth transitions
        float ambient = bound(0.1f, r_lighting_ambient->value, 1.0f);
        if (!useAmbient) ambient = 0.0f;

        // Blend towards fallback lighting
        ambientLight->r = s_lastGoodLight.r * 0.7f + 255 * ambient * 0.3f;
        ambientLight->g = s_lastGoodLight.g * 0.7f + 255 * ambient * 0.3f;
        ambientLight->b = s_lastGoodLight.b * 0.7f + 255 * ambient * 0.3f;
    }

    if (ambientLight->r == 0 && ambientLight->g == 0 && ambientLight->b == 0 && !secondpass)
    {
        secondpass = true;
        goto get_light;
    }

    // Enhanced dynamic lights blending
    if (radius && r_dynamic->integer)
    {
        int lnum, total;
        float f, intensity;

        VectorClear(r_pointColor);

        for (total = lnum = 0, dl = cl_dlights; lnum < MAX_DLIGHTS; lnum++, dl++)
        {
            if (dl->die < cl.time || !dl->radius)
                continue;

            VectorSubtract(dl->origin, point, dir);
            dist = VectorLength(dir);

            if (!dist || dist > dl->radius + radius)
                continue;

            // Enhanced falloff calculation
            add = 1.0f - (dist / (dl->radius + radius));
            add = add * add; // Quadratic falloff for more natural look

            r_pointColor[0] += TextureToTexGamma(dl->color.r) * add;
            r_pointColor[1] += TextureToTexGamma(dl->color.g) * add;
            r_pointColor[2] += TextureToTexGamma(dl->color.b) * add;
            total++;
        }

        if (total != 0)
        {
            // Blend dynamic lights with ambient
            r_pointColor[0] = r_pointColor[0] * 0.8f + ambientLight->r * 0.2f;
            r_pointColor[1] = r_pointColor[1] * 0.8f + ambientLight->g * 0.2f;
            r_pointColor[2] = r_pointColor[2] * 0.8f + ambientLight->b * 0.2f;

            f = max(max(r_pointColor[0], r_pointColor[1]), r_pointColor[2]);
            if (f > 255.0f)
                VectorScale(r_pointColor, (255.0f / f), r_pointColor);

            ambientLight->r = min(r_pointColor[0], 255);
            ambientLight->g = min(r_pointColor[1], 255);
            ambientLight->b = min(r_pointColor[2], 255);

            s_lastGoodLight = *ambientLight;
        }
    }
}

/*
=================
R_GetLightSpot

Enhanced with fallback position
=================
*/
void R_GetLightSpot(vec3_t lightspot)
{
    if (lightspot)
    {
        if (VectorIsNull(r_lightSpot))
            VectorCopy(s_lastGoodLightSpot, lightspot); // Use fallback
        else
            VectorCopy(r_lightSpot, lightspot);
    }
}

/*
=================
R_LightDir

Enhanced with smoother direction calculation
=================
*/
void R_LightDir(const vec3_t origin, vec3_t lightDir, float radius)
{
    dlight_t* dl;
    vec3_t dir, local;
    float dist, weight, totalWeight;
    int lnum;

    VectorClear(local);
    totalWeight = 0.0f;

    // Enhanced dynamic light direction with distance weighting
    if (radius > 0.0f && r_dynamic->integer)
    {
        for (lnum = 0, dl = cl_dlights; lnum < MAX_DLIGHTS; lnum++, dl++)
        {
            if (dl->die < cl.time || !dl->radius)
                continue;

            VectorSubtract(dl->origin, origin, dir);
            dist = VectorLength(dir);

            if (!dist || dist > dl->radius + radius)
                continue;

            // Distance-based weighting for smoother transitions
            weight = 1.0f - (dist / (dl->radius + radius));
            weight = weight * weight; // Quadratic falloff

            VectorMA(local, weight, dir, local);
            totalWeight += weight;
        }

        for (lnum = 0, dl = cl_elights; lnum < MAX_ELIGHTS; lnum++, dl++)
        {
            if (dl->die < cl.time || !dl->radius)
                continue;

            VectorSubtract(dl->origin, origin, dir);
            dist = VectorLength(dir);

            if (!dist || dist > dl->radius + radius)
                continue;

            weight = 1.0f - (dist / (dl->radius + radius));
            weight = weight * weight;

            VectorMA(local, weight, dir, local);
            totalWeight += weight;
        }

        if (totalWeight > 0.0f && !VectorIsNull(local))
        {
            // Normalize by total weight for averaged direction
            VectorScale(local, 1.0f / totalWeight, local);
            VectorNormalize(local);
            VectorCopy(local, lightDir);
            return;
        }
    }

    // Fallback: use upward direction
    VectorSet(lightDir, 0.0f, 0.0f, 1.0f);
}

#endif // XASH_DEDICATED