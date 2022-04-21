#include <stdio.h>
#include <string.h>
#include <dolphin.h>

#include <dolphin/GXEnum.h>
#include "global.h"
#include "gxutil.h"
#include "load.h"
#include "mathutil.h"
#include "ord_tbl.h"
#include "types.h"
#include "tevutil.h"
#include "gma.h"

struct UnkStruct4
{
    u8 filler0[0x18];
    u16 unk18;
    u8 unk1A[4];
    u8 unk1E;
    u8 filler1F[0x40-0x1F];
    u8 unk40[100];  // What type is this?
};

char *invalidModelName = "Invalid Model";

// .sbss
float s_fogEndZ;
float s_fogStartZ;
GXColor s_fogColor;
u32 s_fogType;
s32 s_fogEnabled;
GXColor s_postAddColor;
s32 usePostAddTevStage;
GXColor s_postMultiplyColor;
s32 usePostMultiplyTevStage;
s32 lbl_802F2108;
GXCompare s_zModeCompareFunc;
GXBool s_zModeUpdateEnable;
GXBool s_zModeCompareEnable;
float g_someColorScaleB;
float g_someColorScaleG;
float g_someColorScaleR;
Func802F20F0 g_customMaterialFunc;
BallEnvFunc lbl_802F20EC;
GXCullMode s_cullMode;
float s_boundSphereScale;
u32 s_lightMask;
float s_materialAlpha; // Alpha of MAT in lighting equation: MAT * (RAS + AMB)
float s_ambientBlue; // Blue of AMB in lighting equation: MAT * (RAS + AMB)
float s_ambientGreen; // Green of AMB in lighting equation: MAT * (RAS + AMB)
float s_ambientRed; // Red of AMB of lighting equation: MAT * (RAS + AMB)
Mtx *g_transformMtxList;  // result of matrix multiplications between mtxA and avdispMtxPtrList?
Mtx **avdispMtxPtrList;

// How lighting channel (lighting, vert colors, ambient) is configured
enum
{
    TEVMATCACHE_CHAN_CONFIG_NULL,
    TEVMATCACHE_CHAN_CONFIG_UNLIT,
    TEVMATCACHE_CHAN_CONFIG_UNLIT_VERTCOLORS,
    TEVMATCACHE_CHAN_CONFIG_LIGHTS,
    TEVMATCACHE_CHAN_CONFIG_LIGHTS_VERTCOLORS,
};

struct TevMaterialCache
{
    u8 unk0;  // 0x6c
    s8 tevStageCount;
    u8 chanConfig; // How lighting channel is configured (I think only COLOR0A0 is used)
    s8 num_tev_stages;
    s8 num_tex_gens;
    s8 num_ind_stages;
    u8 filler6[2];
    s32 postMultiplyTevStageIdx;
    s32 postAddTevStageIdx;
    GXColor materialColor;  // 0x7C
    GXColor ambientColor;
    GXColor specularColor;
    s32 colorIn;
    s32 alphaIn;
    GXBlendFactor blendSrcFactor;
    GXBlendFactor blendDstFactor;
    u32 tevLayerFlags[3]; // 0x98  array?
    u32 unk38;  // 0xA4
    u16 tevLayerIdxs[3];  // 0xA8
    s32 unk44;  // 0xB0
    s32 unk48;  // 0xB4
    s32 unk4C;  // 0xB8
    s32 viewSpecularColorValid;  // 0xBC
    s32 worldSpecularColorValid;  // 0xC0
    Vec modelDir_rt_view; // Direction towards front of model (on bounding sphere) in view space
};

// .bss
static Vec lbl_802B4E60;
static Mtx lbl_802B4E6C;
static Mtx lbl_802B4E9C;
static struct TevMaterialCache s_materialCache;
static GXTexObj unknownTexObj;
static u8 filler_802B4F50[0x10];
static u8 lzssHeader[32] __attribute__((aligned(32)));
static u8 unknownTexImg[64];

FORCE_BSS_ORDER(lbl_802B4E60)
FORCE_BSS_ORDER(lbl_802B4E6C)
FORCE_BSS_ORDER(lbl_802B4E9C)
FORCE_BSS_ORDER(s_materialCache)
FORCE_BSS_ORDER(unknownTexObj)
FORCE_BSS_ORDER(filler_802B4F50)
FORCE_BSS_ORDER(lzssHeader)
FORCE_BSS_ORDER(unknownTexImg)

#ifdef __MWERKS__
asm void func_8008D6BC(register u32 arg)
{
    nofralloc
    lis r5, GXWGFifo@h
    li r4, GX_LOAD_BP_REG
    ori r5, r5, GXWGFifo@l
    stb r4, 0(r5)
    stw arg, 0(r5)
    blr
}
#else
void func_8008D6BC(u32 arg)
{
    GXWGFifo.u8 = GX_LOAD_BP_REG;
    GXWGFifo.u32 = arg;
}
#endif

#ifdef __MWERKS__
asm void set_tev_material_ambient_colors(register void *arg)
{
    nofralloc
    lis r10, 0xE0000000@h          // r10 = locked cache
    psq_l f5, 0x1a8(r10), 0, qr2   // f5 = 1.0 | 1.0 (constants stored in locked cache)
    li r6, 0x10                    // r6 = 0x10
    lis r9, GXWGFifo@h
    ori r9, r9, GXWGFifo@l         // r9 = GXWGFifo
    lwz r5, 0(arg)
    andi. r7, r5, 8
    bne lbl_8008D710_if_flag_unk3  // If GMA_SHAPE_FLAG_CUSTOM_MAT_AMB_COLOR is set
    ps_mr f0, f5                   // f0 = 1.0 | 1.0
    ps_mr f1, f5                   // f1 = 1.0 | 1.0
    andi. r7, r5, 0x80
    bne lbl_8008D71C_simple_material
    li r4, -1                      // r4 = 0xFFFFFFFF
    ps_mr f4, f5                   // f4 = 1.0 | 1.0
    b lbl_8008D724
lbl_8008D710_if_flag_unk3:
    psq_l f0, 8(arg), 0, qr2       // f0 = g_color2.rg
    psq_l f1, 10(arg), 1, qr2      // f1 = g_color2.b | 1.0
    ps_merge00 f1, f1, f5          // f1 = g_color2.b | 1.0 (useless?)
lbl_8008D71C_simple_material:
    lhz r4, 4(arg)                 // r4 = g_color1.rg
    psq_l f4, 6(arg), 1, qr2       // f4 = g_color1.b | 1.0
lbl_8008D724:
    lis r7, s_ambientRed@h
    ori r7, r7, s_ambientRed@l        // r7 = &lbl_802F20D0
    psq_l f2, 0(r7), 0, qr0        // f2 = lbl_802F20D0.rg
    psq_l f3, 8(r7), 0, qr0        // f3 = lbl_802F20D0.ba
    ps_mul f0, f0, f2              // f0 = (shape.g_color2.rg OR 1.0|1.0) * lbl_802F20D0.rg
    ps_mul f1, f1, f3              // f1 = shape.g_color2.ba OR 1.0|1.0) * lbl_802F20D0.ba
    psq_l f2, 17(arg), 1, qr2      // f2 = shape.alpha | 1
    ps_merge10 f2, f2, f2          // f2 = 1 | shape.alpha
    psq_st f0, 0x98(r10), 0, qr2   // 
    psq_st f1, 0x9a(r10), 0, qr2   // 
    lwz r7, 0x98(r10)              // r7 = f0.rg f1.ba
    li r8, 0x100a                  // XF_AMBIENT0_ID
    stb r6, 0(r9)                  // Write color in r7 to ambient0 register
    stw r8, 0(r9)                  // I don't think alpha matters here because alpha light channel is always disabled
    stw r7, 0(r9)                  // 
    ps_merge01 f4, f4, f3          // f4 = (shape.g_color1.b OR 1.0) | lbl_802F20D0.a
    ps_mul f4, f4, f2              // f4 = (shape.g_color1.b OR 1.0) | shape.alpha * lbl_802F20D0.a
    sth r4, 0x98(r10)              // (shape.g_color1.rg OR 1.0 | 1.0) -> 0x98(gxCache)
    psq_st f4, 0x9a(r10), 0, qr2   // f4 -> 0x9a(gxCache) as u8s
    lwz r7, 0x98(r10)              // r7 = (original shape.g_color1 with alpha scaled by lbl_802F20D0.a)
    li r8, 0x100c                  // XF_MATERIAL0_ID -> fifo
    stb r6, 0(r9)                  // Write color in r7 to material0 register
    stw r8, 0(r9)
    stw r7, 0(r9)
    blr
}
#else
void set_tev_material_ambient_colors(struct GMAShape *shape)
{
    // TODO
}
#endif

void avdisp_init(void)
{
    Vec sp8;
    lbl_802F20EC = NULL;
    s_ambientBlue = 1.0f;
    s_ambientGreen = 1.0f;
    s_ambientRed = 1.0f;
    s_boundSphereScale = 1.0f;
    s_materialAlpha = 1.0f;
    s_lightMask = 1;
    g_customMaterialFunc = NULL;
    init_some_texture();
    sp8.x = 0.0f;
    sp8.y = 1.0f;
    sp8.z = 0.0f;
    g_avdisp_set_and_normalize_some_vec(&sp8);
    g_avdisp_set_some_color_scale(1.0f, 1.0f, 1.0f);
    avdisp_set_z_mode(GX_ENABLE, GX_LEQUAL, GX_ENABLE);
    lbl_802F2108 = 0;
    mathutil_mtxA_from_translate_xyz(0.0f, 0.0f, 1.0f);
    mathutil_mtxA_to_mtx(lbl_802B4E9C);
    avdisp_set_post_multiply_color(1.0f, 1.0f, 1.0f, 1.0f);
    avdisp_set_post_add_color(0.0f, 0.0f, 0.0f, 0.0f);
    avdisp_enable_fog(0);
    avdisp_set_fog_params(2, 0.0f, 100.0f);
    avdisp_set_fog_color(0, 0, 0);
}

void g_avdisp_alloc_matrix_lists(int count)
{
    g_transformMtxList = OSAlloc(count * sizeof(Mtx));
    avdispMtxPtrList = OSAlloc(count * sizeof(Mtx *));
}

#pragma force_active on
void g_get_stitching_model_mtx(struct GMAModel *model, Mtx **dest)
{
    u8 i;
    int unused1;
    int unused2;
    // matrices are stored immediately after tev layers
    Mtx *pMtx = (void *)((u8 *)model->tevLayers + model->tevLayerCount * sizeof(struct GMATevLayer));

    if (dest == NULL)
    {
        dest = avdispMtxPtrList;
        for (i = 0; i < model->mtxCount; i++)
        {
            *dest = pMtx;
            mathutil_mtx_copy(*pMtx, **dest);  // useless copy source and dest are the same
            pMtx++;
            dest++;
        }
    }
    else
    {
        for (i = 0; i < model->mtxCount; i++)
        {
            mathutil_mtx_copy(*pMtx, **dest);
            pMtx++;
            dest++;
        }
    }
}

struct GMAModel *load_model(char *fileName, struct TPL *tpl)
{
    struct GMAModel *model;
    u32 size;
    struct File file;

    if (!file_open(fileName, &file))
        return NULL;
    size = file_size(&file);
    model = OSAlloc(OSRoundUp32B(size));
    if (model == NULL)
        OSPanic("avdisp.c", 638, "cannot OSAlloc");
    file_read(&file, model, size, 0);
    file_close(&file);
    if (model->magic != (('G' << 24) | ('C' << 16) | ('M' << 8) | 'F'))
    {
        printf("invalid model format <%s>\n", fileName);
        OSFree(model);
        return NULL;
    }
    init_model(model, tpl, NULL);
    return model;
}

void free_model(struct GMAModel *model)
{
    if (model->texObjs != NULL)
        OSFree(model->texObjs);
    OSFree(model);
}
#pragma force_active reset

struct GMA *load_gma(char *fileName, struct TPL *tpl)
{
    int i;
    struct GMA *gma;
    u8 *fileData;
    struct File file;
    int len = strlen(fileName);

    if (len >= 3 && strncmp(fileName + (len - 3), ".lz", 3) == 0)
    {
        void *lzData;
        u32 compressedSize;
        u32 size;

        if (file_open(fileName, &file) == 0)
            return NULL;

        // Read LZSS header
        if (file_read(&file, lzssHeader, 32, 0) < 0)
            OSPanic("avdisp.c", 684, "cannot dvd_read");
        compressedSize = OSRoundUp32B(__lwbrx(lzssHeader, 0));
        size = OSRoundUp32B(__lwbrx(lzssHeader, 4));

        gma = OSAlloc(sizeof(*gma) + size);  // GMA struct followed by raw file data
        if (gma == NULL)
            OSPanic("avdisp.c", 688, "cannot OSAlloc\n");

        // Read whole file
        lzData = OSAlloc(compressedSize);
        if (lzData == NULL)
            OSPanic("avdisp.c", 689, "cannot OSAlooc\n");
        fileData = gma->fileData;
        if (file_read(&file, lzData, compressedSize, 0) < 0)
            OSPanic("avdisp.c", 692, "cannot dvd_read");
        if (file_close(&file) != 1)
            OSPanic("avdisp.c", 693, "cannot DVDClose");

        // Decompress data
        lzs_decompress(lzData, fileData);
        OSFree(lzData);
    }
    else
    {
        u32 size;

        if (file_open(fileName, &file) == 0)
            return NULL;
        size = file_size(&file);
        gma = OSAlloc(sizeof(*gma) + OSRoundUp32B(size));  // GMA struct followed by raw file data
        if (gma == NULL)
            OSPanic("avdisp.c", 702, "cannot OSAlloc");
        fileData = gma->fileData;
        file_read(&file, fileData, size, 0);
        file_close(&file);
    }

    // Read GMA header
    gma->numModels = *(u32 *)(fileData + 0);  // 0: number of models
    gma->modelsBase = OFFSET_TO_PTR(fileData, *(u32 *)(fileData + 4));  // 4: offset to model data
    gma->modelEntries = OFFSET_TO_PTR(fileData, 8);  // 8: model entries
    gma->namesBase = (char *)(gma->modelEntries + *(u32 *)(fileData + 0));  // names base follows model entries

    // Load models
    for (i = 0; i < gma->numModels; i++)
    {
        struct GMAModelEntry *entry = &gma->modelEntries[i];
        void *offset = entry->modelOffset;

        if ((u32)entry->modelOffset == 0xFFFFFFFF)
        {
            entry->modelOffset = NULL;
            entry->name = invalidModelName;
        }
        else
        {
            // Convert name and model offsets to pointers
            offset = OFFSET_TO_PTR(gma->modelsBase, (u32)offset);
            entry->modelOffset = offset;
            entry->name = OFFSET_TO_PTR(gma->namesBase, (u32)entry->name);
            // Load the model
            init_model(offset, tpl, NULL);
        }
    }
    return gma;
}

struct GMA *load_gma_from_aram(u32 aramSrc, u32 size, struct TPL *tpl)
{
    u32 alignedSize = OSRoundUp32B(size);
    int i;
    u8 *fileData;

    struct GMA *gma = OSAlloc(sizeof(*gma) + alignedSize);  // GMA struct followed by raw file data
    if (gma == NULL)
        OSPanic("avdisp.c", 750, "cannot OSAlloc");
    fileData = gma->fileData;
    DCInvalidateRange(fileData, alignedSize);
    while (ARGetDMAStatus() != 0)
        ;
    ARStartDMA(ARAM_DIR_ARAM_TO_MRAM, (u32)fileData, aramSrc, size);
    while (ARGetDMAStatus() != 0)
        ;

    // Read GMA header
    gma->numModels = *(u32 *)(fileData + 0);  // 0: number of models
    gma->modelsBase = OFFSET_TO_PTR(fileData, *(u32 *)(fileData + 4));  // 4: offset to model data
    gma->modelEntries = OFFSET_TO_PTR(fileData, 8);  // 8: model entries
    gma->namesBase = (char *)(gma->modelEntries + *(u32 *)(fileData + 0));  // names base follows model entries

    // Load models
    for (i = 0; i < gma->numModels; i++)
    {
        struct GMAModelEntry *entry = &gma->modelEntries[i];
        void *offset = entry->modelOffset;

        if ((u32)entry->modelOffset == 0xFFFFFFFF)
        {
            entry->modelOffset = NULL;
            entry->name = invalidModelName;
        }
        else
        {
            // Convert name and model offsets to pointers
            offset = OFFSET_TO_PTR(gma->modelsBase, (u32)offset);
            entry->modelOffset = offset;
            entry->name = OFFSET_TO_PTR(gma->namesBase, (u32)entry->name);
            // Load the model
            init_model(offset, tpl, NULL);
        }
    }
    return gma;
}

void free_gma(struct GMA *gma)
{
    int i;

    for (i = 0; i < (s32)gma->numModels; i++)
    {
        struct GMAModel *model = gma->modelEntries[i].modelOffset;

        if (model != NULL && model->texObjs != NULL)
            OSFree(model->texObjs);
    }
    OSFree(gma);
}

struct TPL *load_tpl(char *fileName)
{
    void *lzData;
    struct TPL *tpl;
    u8 *fileData;
    struct File file;
    int len = strlen(fileName);

    if (len >= 3 && strncmp(fileName + (len - 3), ".lz", 3) == 0)
    {
        u32 compressedSize;
        u32 size;

        if (file_open(fileName, &file) == 0)
            return NULL;

        // Read LZSS header
        if (file_read(&file, lzssHeader, 32, 0) < 0)
            OSPanic("avdisp.c", 822, "cannot dvd_read");
        compressedSize = OSRoundUp32B(__lwbrx(lzssHeader, 0));
        size = OSRoundUp32B(__lwbrx(lzssHeader, 4));

        tpl = OSAlloc(12 + size);  // TPL struct followed by raw file data
        if (tpl == NULL)
            OSPanic("avdisp.c", 826, "cannot OSAlloc\n");
        lzData = OSAlloc(compressedSize);
        if (lzData == NULL)
            OSPanic("avdisp.c", 827, "cannot OSAlooc\n");

        // Read whole file
        fileData = (void *)OSRoundUp32B((u32)tpl + 12);
        if (file_read(&file, lzData, compressedSize, 0) < 0)
            OSPanic("avdisp.c", 830, "cannot dvd_read");
        if (file_close(&file) != 1)
            OSPanic("avdisp.c", 831, "cannot DVDClose");

        // Decompress data
        lzs_decompress(lzData, fileData);
        OSFree(lzData);
    }
    else
    {
        u32 size;
        if (file_open(fileName, &file) == 0)
            return NULL;
        size = file_size(&file);
        tpl = OSAlloc(OSRoundUp32B(size) + 32);  // TPL struct followed by raw file data
        if (tpl == NULL)
            OSPanic("avdisp.c", 840, "cannot OSAlloc");
        fileData = (void *)OSRoundUp32B((u32)tpl + 12);
        file_read(&file, fileData, size, 0);
        file_close(&file);
    }

    tpl->numTextures = *(u32 *)(fileData + 0);  // 0: number of textures
    tpl->texHeaders = OFFSET_TO_PTR(fileData, 4);  // 4: texture headers
    tpl->fileData = fileData;
    return tpl;
}

struct TPL *load_tpl_from_aram(u32 aramSrc, u32 size)
{
    u32 alignedSize = OSRoundUp32B(size);
    struct TPL *tpl;
    u8 *fileData;

    tpl = OSAlloc(alignedSize + 32);
    if (tpl == NULL)
        OSPanic("avdisp.c", 865, "cannot OSAlloc");
    fileData = (void *)OSRoundUp32B((u32)tpl + 12);
    DCInvalidateRange(fileData, alignedSize);
    while (ARGetDMAStatus() != 0)
        ;
    ARStartDMA(ARAM_DIR_ARAM_TO_MRAM, (u32)fileData, aramSrc, size);
    while (ARGetDMAStatus() != 0)
        ;

    tpl->numTextures = *(u32 *)(fileData + 0);  // 0: number of textures
    tpl->texHeaders = OFFSET_TO_PTR(fileData, 4);  // 4: texture headers
    tpl->fileData = fileData;
    return tpl;
}

GXTexObj *create_tpl_tex_objs(struct TPL *tpl)
{
    int i;
    struct TPLTextureHeader *texHdrs = tpl->texHeaders;
    GXTexObj *texObjs = OSAlloc(tpl->numTextures * sizeof(GXTexObj));

    for (i = 0; i < tpl->numTextures; i++)
    {
        u8 maxLOD;
        void *imagePtr = tpl->fileData + texHdrs[i].imageOffset;
        if (texHdrs[i].width != texHdrs[i].height)
            maxLOD = 0;
        else
            maxLOD = get_texture_max_lod(texHdrs[i].width, texHdrs[i].height);
        GXInitTexObj(
            &texObjs[i],               // obj
            (void *)imagePtr,          // image_ptr
            texHdrs[i].width,          // width
            texHdrs[i].height,         // height
            texHdrs[i].format & 0x1F,  // format
            GX_REPEAT,                 // wrap_s
            GX_REPEAT,                 // wrap_t
            0);                        // mipmap
        GXInitTexObjLOD(
            &texObjs[i],  // obj
            (maxLOD != 0) ? GX_LIN_MIP_LIN : GX_LIN_MIP_NEAR,  // min_filt
            GX_LINEAR,    // mag_filt
            0.0f,         // min_lod
            maxLOD,       // max_lod
            0.0f,         // lod_bias
            GX_FALSE,     // bias_clamp
            TRUE,         // do_edge_lod
            GX_ANISO_1);  // max_aniso
    }
    return texObjs;
}

void free_tpl(struct TPL *tpl)
{
    OSFree(tpl);
}

void avdisp_set_bound_sphere_scale(float a)
{
    s_boundSphereScale = a;
}

void avdisp_set_ambient(float red, float green, float blue)
{
    s_ambientRed = red;
    s_ambientGreen = green;
    s_ambientBlue = blue;
}

// Draw opaque shapes immediately and depth-sort translucent shapes, a reasonable default
void avdisp_draw_model_culled_sort_translucent(struct GMAModel *model)
{
    if (g_test_scaled_sphere_in_frustum(&model->boundSphereCenter, model->boundSphereRadius, s_boundSphereScale) == 0)
    {
        s_boundSphereScale = 1.0f;
        GXSetCurrentMtx(GX_PNMTX0);
        s_materialAlpha = 1.0f;
    }
    else
        avdisp_draw_model_unculled_sort_translucent(model);
}

// Draw both opaque and translucent shapes immediately. Useful when you want to explicitly draw
// translucent shapes in a specific order instead of using the ord table?
void avdisp_draw_model_culled_sort_none(struct GMAModel *model)
{
    if (g_test_scaled_sphere_in_frustum(&model->boundSphereCenter, model->boundSphereRadius, s_boundSphereScale) == 0)
    {
        s_boundSphereScale = 1.0f;
        GXSetCurrentMtx(GX_PNMTX0);
        s_materialAlpha = 1.0f;
    }
    else
        avdisp_draw_model_unculled_sort_none(model);
}

// Depth-sort both opaque and translucent shapes. Useful if your opaque shapes become translucent
// e.g. by calling avdisp_set_alpha() ?
void avdisp_draw_model_culled_sort_all(struct GMAModel *model)
{
    if (g_test_scaled_sphere_in_frustum(&model->boundSphereCenter, model->boundSphereRadius, s_boundSphereScale) == 0)
    {
        s_boundSphereScale = 1.0f;
        GXSetCurrentMtx(GX_PNMTX0);
        s_materialAlpha = 1.0f;
    }
    else
        avdisp_draw_model_unculled_sort_all(model);
}

void avdisp_set_alpha(float a)
{
    s_materialAlpha = a;
}

void avdisp_set_light_mask(u32 a)
{
    s_lightMask = a;
}

void g_avdisp_set_and_normalize_some_vec(Vec *a)
{
    lbl_802B4E60 = *a;
    mathutil_vec_normalize_len(&lbl_802B4E60);
}

void g_avdisp_set_some_color_scale(float a, float b, float c)
{
    g_someColorScaleR = a;
    g_someColorScaleG = b;
    g_someColorScaleB = c;
}

void avdisp_set_z_mode(GXBool compareEnable, GXCompare compareFunc, GXBool updateEnable)
{
    s_zModeCompareEnable = compareEnable;
    s_zModeCompareFunc   = compareFunc;
    s_zModeUpdateEnable  = updateEnable;
}

BallEnvFunc g_avdisp_set_some_func_1(BallEnvFunc func)
{
    BallEnvFunc old = lbl_802F20EC;
    lbl_802F20EC = func;
    return old;
}

Func802F20F0 g_avdisp_set_some_func_2(Func802F20F0 func)
{
    Func802F20F0 old = g_customMaterialFunc;
    g_customMaterialFunc = func;
    return old;
}

struct UnkStruct12
{
    u8 filler0[8];
    u32 unk8;
};

#pragma force_active on
void *func_8008E5F8(struct GMAModel *model)
{
    struct UnkStruct12 *r3 = (struct UnkStruct12 *)((u8 *)model + model->headerSize);
    if (model->flags & (GCMF_SKIN|GCMF_EFFECTIVE))
        return (u8 *)r3 + r3->unk8;
    else
    {
        printf("non effective model.\n");
        return NULL;
    }
}

void *func_8008E64C(struct GMAModel *model)
{
    // hmm... with these flags, this shouldn't point to the shape?
    if (model->flags & (GCMF_SKIN|GCMF_EFFECTIVE))
        return (u8 *)model + model->headerSize;
    else
    {
        printf("non effective model.\n");
        return NULL;
    }
}
#pragma force_active reset

// Returns the shape that follows after the given shape and all of its display lists
static inline struct GMAShape *next_shape(struct GMAShape *shape)
{
    u8 *ptr = shape->dispLists;
    int i;

    // Skip over first two display lists
    for (i = 0; i < 2; i++)
    {
        if (shape->dispListFlags & (1 << i))
            ptr += shape->dispListSizes[i];
    }

    // Skip over extra display lists
    if (shape->dispListFlags & (GMA_SHAPE_HAS_DLIST2 | GMA_SHAPE_HAS_DLIST3))
    {
        u32 *r4 = (u32 *)ptr;
        ptr += 32;
        ptr += r4[2];
        ptr += r4[3];
    }

    return (void *)ptr;
}

#pragma force_active on
void set_shape_flags_in_model(struct GMAModel *model, u32 flags)
{
    struct GMAShape *shapePtr = OFFSET_TO_PTR(model, model->headerSize);

    if (model->flags & (GCMF_SKIN|GCMF_EFFECTIVE))
    {
        u32 i;
        struct GMAShape *shapes = OFFSET_TO_PTR(shapePtr, 32);
        for (i = 0; i < model->opaqueShapeCount + model->translucentShapeCount; i++)
            shapes[i].flags |= flags;
    }
    else
    {
        u32 i;
        struct GMAShape *shape = shapePtr;
        for (i = 0; i < model->opaqueShapeCount + model->translucentShapeCount; i++)
        {
            shape->flags |= flags;
            shape = next_shape(shape);
        }
    }
}
#pragma force_active reset

struct DrawShapeDeferredNode
{
    struct OrdTblNode node;
    struct GMAModel *model;
    Mtx mtx;
    struct GMATevLayer *modelSamplers;
    struct GMAShape *shape;
    GXCullMode cullMode;
    u32 unk48;
    float alpha;
    BallEnvFunc unk50;
    Func802F20F0 unk54;
    u8 zCompEnable;
    u8 zUpdEnable;
    u32 zCompFunc;
    u8 unk60;
    u8 usePostMultiplyTevStage;
    u8 usePostAddTevStage;
    Mtx *unk64;
    GXColor unk68;
    GXColor unk6C;
    u32 fogEnabled;
};

static inline struct GMAShape *draw_shape_deferred(struct GMAModel *model, struct GMAShape *shape, struct GMATevLayer *modelSamplers)
{
    struct OrdTblNode *entry;
    GXCullMode cullMode = s_cullMode;
    struct DrawShapeDeferredNode *node = ord_tbl_alloc_node(sizeof(*node));

    if (shape->unk14 != 0xFF)
        entry = ord_tbl_get_entry_for_pos_offset_index(&shape->origin, -1);
    else
        entry = ord_tbl_get_entry_for_pos(&shape->origin);
    node->node.drawFunc = (OrdTblDrawFunc)draw_shape_deferred_callback;
    node->model = model;
    node->shape = shape;
    node->modelSamplers = modelSamplers;
    node->cullMode = cullMode;
    node->unk48 = func_800223D0();
    node->alpha = s_materialAlpha;
    node->unk50 = lbl_802F20EC;
    node->unk54 = g_customMaterialFunc;
    node->zCompEnable = s_zModeCompareEnable;
    node->zUpdEnable = s_zModeUpdateEnable;
    node->zCompFunc = s_zModeCompareFunc;
    node->unk60 = lbl_802F2108;
    node->usePostMultiplyTevStage = usePostMultiplyTevStage;
    node->usePostAddTevStage = usePostAddTevStage;
    if (node->unk60 != 0)
    {
        node->unk64 = ord_tbl_alloc_node(sizeof(*node->unk64));
        mathutil_mtx_copy(lbl_802B4E6C, *node->unk64);
    }
    if (node->usePostMultiplyTevStage)
        node->unk68 = s_postMultiplyColor;
    if (node->usePostAddTevStage)
        node->unk6C = s_postAddColor;
    node->fogEnabled = s_fogEnabled;
    mathutil_mtxA_to_mtx(node->mtx);
    ord_tbl_insert_node(entry, &node->node);
    return next_shape(shape);
}

// Draw opaque shapes immediately and depth-sort translucent shapes, a reasonable default
void avdisp_draw_model_unculled_sort_translucent(struct GMAModel *model)
{
    struct GMAShape *shape = OFFSET_TO_PTR(model, model->headerSize);
    struct GMATevLayer *modelSamplers = OFFSET_TO_PTR(model, 0x40);
    int i;

    s_cullMode = GX_CULL_BACK;
    GXSetCullMode_cached(s_cullMode);
    if (model->flags & GCMF_STITCHING)
        g_iteratively_multiply_model_matrices(model);
    if (g_customMaterialFunc == NULL)
        init_tev_material_cache(model, shape);

    if (model->flags & (GCMF_SKIN|GCMF_EFFECTIVE))
        draw_indexed_model(model, shape, modelSamplers);
    else
    {
        for (i = 0; i < model->opaqueShapeCount; i++)
            shape = draw_shape(model, shape, modelSamplers);
        for (i = 0; i < model->translucentShapeCount; i++)
            shape = draw_shape_deferred(model, shape, modelSamplers);
    }

    s_boundSphereScale = 1.0f;
    GXSetCurrentMtx(GX_PNMTX0);
    s_materialAlpha = 1.0f;
}

// Draw both opaque and translucent shapes immediately. Useful when you want to explicitly draw
// translucent shapes in a specific order instead of using the ord table?
void avdisp_draw_model_unculled_sort_none(struct GMAModel *model)
{
    struct GMAShape *shape = OFFSET_TO_PTR(model, model->headerSize);
    struct GMATevLayer *modelSamplers = OFFSET_TO_PTR(model, 0x40);
    int i;

    s_cullMode = 2;
    GXSetCullMode_cached(s_cullMode);
    if (model->flags & GCMF_STITCHING)
        g_iteratively_multiply_model_matrices(model);
    if (g_customMaterialFunc == NULL)
        init_tev_material_cache(model, shape);

    if (model->flags & (GCMF_SKIN|GCMF_EFFECTIVE))
        draw_indexed_model(model, shape, modelSamplers);
    else
    {
        for (i = 0; i < model->opaqueShapeCount; i++)
            shape = draw_shape(model, shape, modelSamplers);
        for (i = 0; i < model->translucentShapeCount; i++)
            shape = draw_shape(model, shape, modelSamplers);
    }

    s_boundSphereScale = 1.0f;
    GXSetCurrentMtx(GX_PNMTX0);
    s_materialAlpha = 1.0f;
}

// Depth-sort both opaque and translucent shapes. Useful if your opaque shapes become translucent
// e.g. by calling avdisp_set_alpha() ?
void avdisp_draw_model_unculled_sort_all(struct GMAModel *model)
{
    struct GMAShape *shape = OFFSET_TO_PTR(model, model->headerSize);
    struct GMATevLayer *modelSamplers = OFFSET_TO_PTR(model, 0x40);
    int i;

    s_cullMode = 2;
    GXSetCullMode_cached(s_cullMode);
    if (model->flags & GCMF_STITCHING)
        g_iteratively_multiply_model_matrices(model);
    if (g_customMaterialFunc == NULL)
        init_tev_material_cache(model, shape);

    for (i = 0; i < model->opaqueShapeCount; i++)
        shape = (void *)draw_shape_deferred(model, shape, modelSamplers);
    for (i = 0; i < model->translucentShapeCount; i++)
        shape = (void *)draw_shape_deferred(model, shape, modelSamplers);

    s_boundSphereScale = 1.0f;
    GXSetCurrentMtx(GX_PNMTX0);
    s_materialAlpha = 1.0f;
}

// Used by shadows
void g_avdisp_draw_model_4(struct GMAModel *model)
{
    struct GMAShape *shape = OFFSET_TO_PTR(model, model->headerSize);
    struct GMATevLayer *modelSamplers = OFFSET_TO_PTR(model, 0x40);
    int i;

    s_cullMode = 2;
    if (model->flags & GCMF_STITCHING)
        g_iteratively_multiply_model_matrices(model);

    if (model->flags & (GCMF_SKIN|GCMF_EFFECTIVE))
        draw_indexed_model(model, shape, modelSamplers);
    else
    {
        for (i = 0; i < model->opaqueShapeCount; i++)
            shape = draw_shape(model, shape, modelSamplers);
    }

    s_boundSphereScale = 1.0f;
    GXSetCurrentMtx(GX_PNMTX0);
    s_materialAlpha = 1.0f;
}

int get_texture_max_lod(int width, int height)
{
    int lod;
    if (width > height)
        width = height;
    lod = 0;
    while (width > 16)
    {
        width >>= 1;
        lod++;
    }
    return lod;
}

void init_tev_layer_texobj(struct GMATevLayer *tevLayer, struct TPLTextureHeader *texHdr, struct TPL *tpl)
{
    u8 maxLOD;
    int wrapS;
    int wrapT;
    void *imagePtr = tpl->fileData + texHdr->imageOffset;
    int minFilt;
    int magFilt;

    if (texHdr->format & 0x40)
    {
        OSPanic("avdisp.c", 1340, "invalid texture!!\n...stopped\n");
        tevLayer->texObj = NULL;
        return;
    }

    switch ((tevLayer->flags >> 2) & 3)
    {
    case 2:
        wrapS = GX_MIRROR;
        break;
    case 1:
        wrapS = GX_REPEAT;
        break;
    default:
        wrapS = GX_CLAMP;
        break;
    }
    switch ((tevLayer->flags >> 4) & 3)
    {
    case 2:
        wrapT = GX_MIRROR;
        break;
    case 1:
        wrapT = GX_REPEAT;
        break;
    default:
        wrapT = GX_CLAMP;
        break;
    }
    if (texHdr->width != texHdr->height)
        maxLOD = 0;
    else
    {
        maxLOD = (tevLayer->flags >> 7) & 0xF;
        if (maxLOD == 15)
            maxLOD = get_texture_max_lod(texHdr->width, texHdr->height);  // inlined
    }
    GXInitTexObj(
        tevLayer->texObj,  // obj
        imagePtr,  // image_ptr
        texHdr->width,  // width
        texHdr->height,  // height
        texHdr->format & 0x1F,  // format
        wrapS,  // wrap_s
        wrapT,  // wrap_t
        maxLOD != 0);  // mipmap

    if (tevLayer->flags & GMA_TEV_LAYER_FLAG_MAGFILT_NEAR)
    {
        magFilt = GX_NEAR;
        if (maxLOD != 0)
            minFilt = GX_NEAR_MIP_LIN;
        else
            minFilt = GX_NEAR_MIP_NEAR;
    }
    else
    {
        magFilt = GX_LINEAR;
        if (maxLOD != 0)
            minFilt = GX_LIN_MIP_LIN;
        else
            minFilt = GX_LIN_MIP_NEAR;
    }
    GXInitTexObjLOD(
        tevLayer->texObj,  // obj
        minFilt,  // min_filt
        magFilt,  // mag_filt
        0.0f,  // min_lod
        maxLOD,  // max_lod
        tevLayer->lodBias / 10.0f,  // lod_bias
        FALSE,  // bias_clamp
        (tevLayer->flags & GMA_TEV_LAYER_FLAG_DO_EDGE_LOD) != 0,  // do_edge_lod
        tevLayer->maxAniso);  // max_aniso
}

static inline struct GMAShape *init_shape_render_flags(struct GMAShape *shape)
{
    if (shape->vtxAttrs & (1 << GX_VA_CLR0))
        shape->flags |= GMA_SHAPE_FLAG_VERT_COLORS;
    if (shape->tevLayerCount == 0)
        shape->flags |= GMA_SHAPE_FLAG_SIMPLE_MATERIAL;
    return next_shape(shape);
}

GXTexObj *init_model(struct GMAModel *model, struct TPL *tpl, GXTexObj *texObj)
{
    struct GMATevLayer *modelSamplers;
    struct GMAShape *shape;
    int i;

    if (tpl == NULL)
        model->tevLayerCount = 0;
    modelSamplers = (void *)model->tevLayers;

    // Allocate GX texture objects for tev layers
    if (model->tevLayerCount != 0)
    {
        if (texObj != NULL)
        {
            model->texObjs = texObj;
            texObj += model->tevLayerCount;
        }
        else
            model->texObjs = OSAlloc(model->tevLayerCount * sizeof(GXTexObj));
        if (model->texObjs == NULL)
            OSPanic("avdisp.c", 0x58B, "cannot OSAlloc");
    }
    else
        model->texObjs = NULL;

    // Initialize tev layers
    for (i = 0; i < model->tevLayerCount; i++)
    {
        modelSamplers[i].texObj = &model->texObjs[i];
        init_tev_layer_texobj(&modelSamplers[i], &tpl->texHeaders[modelSamplers[i].unk4], tpl);
    }

    // Get pointer to shape
    // If model is GCMF_SKIN or GCMF_EFFECTIVE, then the shape offset is 32 bytes more
    shape = OFFSET_TO_PTR(model, model->headerSize);
    if (model->flags & (GCMF_SKIN|GCMF_EFFECTIVE))
        shape = OFFSET_TO_PTR((u32)shape, 32);

    for (i = 0; i < model->opaqueShapeCount; i++)
    {
        if (tpl == NULL)
            shape->tevLayerCount = 0;
        if (model->flags & (GCMF_SKIN|GCMF_EFFECTIVE))
        {
            struct GMAShape *r3 = shape;
            shape++;
            init_shape_render_flags(r3);
        }
        else
            shape = init_shape_render_flags(shape);
    }
    for (i = 0; i < model->translucentShapeCount; i++)
    {
        if (tpl == NULL)
            shape->tevLayerCount = 0;
        if (model->flags & (GCMF_SKIN|GCMF_EFFECTIVE))
        {
            struct GMAShape *r3 = shape;
            shape++;
            init_shape_render_flags(r3);
        }
        else
            shape = init_shape_render_flags(shape);
    }
    return texObj;
}

void g_iteratively_multiply_model_matrices(struct GMAModel *model)
{
    unsigned int i;

    for (i = 0; i < model->mtxCount; i++)
        mathutil_mtx_mult(mathutilData->mtxA, *avdispMtxPtrList[i], g_transformMtxList[i]);
    func_8008F8A4(model->mtxIndexes);
}

void draw_shape_deferred_callback(struct DrawShapeDeferredNode *node)
{
    BallEnvFunc r31;
    Func802F20F0 r30;
    GXBool zModeCompareEnable;
    GXBool zModeUpdateEnable;
    GXCompare zModeCompareFunc;
    u32 r26;
    u32 r25;
    u32 r23;
    u32 fogEnabled;
    GXColor sp10;
    GXColor spC;

    if ((node->shape->flags & GMA_SHAPE_FLAG_UNLIT) == 0)
        func_800223D8(node->unk48);
    g_gxutil_upload_some_mtx(node->mtx, 0);
    mathutil_mtxA_from_mtx(node->mtx);
    GXSetCullMode_cached(node->cullMode);
    s_cullMode = node->cullMode;
    r31 = lbl_802F20EC;
    r30 = g_customMaterialFunc;
    zModeCompareEnable = s_zModeCompareEnable;
    zModeUpdateEnable = s_zModeUpdateEnable;
    zModeCompareFunc = s_zModeCompareFunc;
    r26 = lbl_802F2108;
    s_materialAlpha = node->alpha;
    lbl_802F20EC = node->unk50;
    g_customMaterialFunc = node->unk54;
    s_zModeCompareEnable = node->zCompEnable;
    s_zModeUpdateEnable = node->zUpdEnable;
    s_zModeCompareFunc = node->zCompFunc;
    lbl_802F2108 = node->unk60;
    if (node->unk60 != 0)
        mathutil_mtx_copy(*node->unk64, lbl_802B4E6C);
    r25 = usePostMultiplyTevStage;
    usePostMultiplyTevStage = node->usePostMultiplyTevStage;
    if (node->usePostMultiplyTevStage != 0)
    {
        sp10 = s_postMultiplyColor;
        s_postMultiplyColor = node->unk68;
    }
    r23 = usePostAddTevStage;
    usePostAddTevStage = node->usePostAddTevStage;
    if (node->usePostAddTevStage != 0)
    {
        spC = s_postAddColor;
        s_postAddColor = node->unk6C;
    }
    fogEnabled = s_fogEnabled;
    s_fogEnabled = node->fogEnabled;
    if (g_customMaterialFunc == NULL)
        init_tev_material_cache(node->model, node->shape);
    draw_shape(node->model, node->shape, node->modelSamplers);
    lbl_802F20EC = r31;
    g_customMaterialFunc = r30;
    s_zModeCompareEnable = zModeCompareEnable;
    s_zModeUpdateEnable = zModeUpdateEnable;
    s_zModeCompareFunc = zModeCompareFunc;
    lbl_802F2108 = r26;
    usePostMultiplyTevStage = r25;
    if (node->usePostMultiplyTevStage != 0)
        s_postMultiplyColor = sp10;
    usePostAddTevStage = r23;
    if (node->usePostAddTevStage != 0)
        s_postAddColor = spC;
    s_fogEnabled = fogEnabled;
    s_materialAlpha = 1.0f;
}

u32 g_avdisp_set_some_tex_mtx_sel(u32 a)
{
    u32 old = lbl_802F2108;
    lbl_802F2108 = a;
    return old;
}

void g_avdisp_set_some_matrix(int unused, Mtx mtx)
{
    mathutil_mtx_copy(mtx, lbl_802B4E6C);
}

void avdisp_set_post_multiply_color(float r, float g, float b, float a)
{
    if (r != 1.0f || g != 1.0f || b != 1.0f || a != 1.0f)
    {
        usePostMultiplyTevStage = 1;
        s_postMultiplyColor.r = r * 255.0f;
        s_postMultiplyColor.g = g * 255.0f;
        s_postMultiplyColor.b = b * 255.0f;
        s_postMultiplyColor.a = a * 255.0f;
    }
    else
    {
        usePostMultiplyTevStage = 0;
        s_postMultiplyColor.r = 255;
        s_postMultiplyColor.g = 255;
        s_postMultiplyColor.b = 255;
        s_postMultiplyColor.a = 255;
    }
}

void avdisp_set_post_add_color(float r, float g, float b, float a)
{
    if (r != 0.0f || g != 0.0f || b != 0.0f || a != 0.0f)
    {
        usePostAddTevStage = 1;
        s_postAddColor.r = r * 255.0f;
        s_postAddColor.g = g * 255.0f;
        s_postAddColor.b = b * 255.0f;
        s_postAddColor.a = a * 255.0f;
    }
    else
    {
        usePostAddTevStage = 0;
        s_postAddColor.r = 0;
        s_postAddColor.g = 0;
        s_postAddColor.b = 0;
        s_postAddColor.a = 0;
    }
}

void avdisp_enable_fog(int a)
{
    s_fogEnabled = a;
}

void avdisp_set_fog_params(int a, float b, float c)
{
    s_fogType = a;
    s_fogStartZ = b;
    s_fogEndZ = c;
}

void avdisp_set_fog_color(u8 a, u8 b, u8 c)
{
    s_fogColor.r = a;
    s_fogColor.g = b;
    s_fogColor.b = c;
}

void func_8008F8A4(u8 *mtxIndexes)
{
    int i;
    for (i = 0; i < 8; i++)
    {
        if ((s32)mtxIndexes[i] != 0xFF)
            g_gxutil_upload_some_mtx(g_transformMtxList[mtxIndexes[i]], i + 1);
    }
}


struct UnkStruct27
{
    u8 filler0[4];
    struct GMAShape *shape;
    struct GMATevLayer *modelTevs;
    u8 fillerC[0x38-0xC];
};

// Returns pointer to shape that follows this one
struct GMAShape *draw_shape(struct GMAModel *model, struct GMAShape *shape, struct GMATevLayer *modelTevs)
{
    int i;
    u8 *dlist;
    GXCullMode cullMode;
    u8 doDraw;
    struct UnkStruct27 sp20;
    u8 unused[12];

    if (shape->flags & GMA_SHAPE_FLAG_DOUBLE_SIDED)
        cullMode = GX_CULL_NONE;
    else
        cullMode = GX_CULL_FRONT;
    if (model->flags & GCMF_STITCHING)
        func_8008F8A4(shape->mtxIndices);  // inlined
    gxutil_set_vtx_attrs(shape->vtxAttrs);
    dlist = shape->dispLists;

    if (g_customMaterialFunc != NULL)
    {
        sp20.shape = (void *)shape;
        sp20.modelTevs = modelTevs;
        doDraw = g_customMaterialFunc(&sp20);
    }
    else
    {
        build_tev_material(shape, modelTevs);
        doDraw = 1;
    }

    if (doDraw)
    {
        for (i = 0; i < 2; i++)
        {
            if (shape->dispListFlags & (1 << i))
            {
                if (s_cullMode != cullMode)
                {
                    s_cullMode = cullMode;
                    GXSetCullMode_cached(cullMode);
                }
                GXCallDisplayList(dlist, shape->dispListSizes[i]);
                dlist += shape->dispListSizes[i];
            }
            if (cullMode != GX_CULL_NONE)
                cullMode = GX_CULL_BACK;
        }
        if (shape->dispListFlags & (GMA_SHAPE_HAS_DLIST2 | GMA_SHAPE_HAS_DLIST3))
        {
            struct GMAExtraDispLists *extraDispLists = (void *)dlist;
            func_8008F8A4(extraDispLists->mtxIndices);  // inlined
            dlist = extraDispLists->dlists;
            for (i = 0; i < 2; i++)
            {
                if (i == 0)
                    GXSetCullMode_cached(GX_CULL_FRONT);
                else
                    GXSetCullMode_cached(GX_CULL_BACK);
                GXCallDisplayList(dlist, extraDispLists->dispListSizes[i]);
                dlist += extraDispLists->dispListSizes[i];
            }
        }
    }
    else
    {
        return next_shape(shape);
    }
    return (struct GMAShape *)dlist;
}

#ifdef __MWERKS__
asm void func_8008FBB0(register u32 _flags, register void *_base, void *c, u32 d)
{
    nofralloc
    lis r9, GXWGFifo@h
    ori r9, r9, GXWGFifo@l  // r9 = GX FIFO
    addi r5, r5, -4
next_strip:
    lwzu r7, 4(r5)  // vertex count
    addi r8, r7, 1
    li r10, GX_DRAW_TRIANGLE_STRIP
    stb r10, 0(r9)
    sth r7, 0(r9)
next_tri:
    lwzu r11, 4(r5)
    add r10, _base, r11
    // copy vertex
    psq_l f0, 0(r10), 0, qr0
    psq_l f1, 8(r10), 1, qr0
    psq_st f0, 0(r9), 0, qr0
    psq_st f1, 0(r9), 1, qr0
    // copy vertex
    psq_l f2, 12(r10), 0, qr0
    psq_l f3, 20(r10), 1, qr0
    psq_st f2, 0(r9), 0, qr0
    psq_st f3, 0(r9), 1, qr0
    extrwi. r12, r3, 1, (31-11)
    beq+ lbl_8008FC08
    // if flags & (1<<11)
    psq_l f0, 48(r10), 1, qr0
    psq_st f0, 0(r9), 1, qr0
lbl_8008FC08:
    extrwi. r12, _flags, 1, (31-13)
    beq lbl_8008FC18
    // if flags & (1<<13)
    psq_l f0, 24(r10), 0, qr0
    psq_st f0, 0(r9), 0, qr0
lbl_8008FC18:
    extrwi. r12, _flags, 1, (31-14)
    beq+ lbl_8008FC28
    // if flags & (1<<14)
    psq_l f1, 32(r10), 0, qr0
    psq_st f1, 0(r9), 0, qr0
lbl_8008FC28:
    extrwi. r12, _flags, 1, (31-15)
    beq+ lbl_8008FC38
    // if flags & (1<<15)
    psq_l f2, 40(r10), 0, qr0
    psq_st f2, 0(r9), 0, qr0
lbl_8008FC38:
    addic. r7, r7, -1
    bgt next_tri
    subf. r6, r8, r6
    bgt next_strip
    blr
}
#else
void func_8008FBB0(u32 _flags, void *_base, void *c, u32 d)
{
    // TODO
}
#endif

struct UnkStruct29
{
    u8 filler0[8];
    u32 unk8;
};

void *draw_shape_reflection_maybe(struct GMAShape *shape, void *modelSamplers, struct UnkStruct29 *c, u8 *d)
{
    int i;
    GXCullMode cullMode;
    u8 *r29 = (u8 *)c + c->unk8;
    u8 bvar;
    struct UnkStruct27 sp20;

    if (shape->flags & GMA_SHAPE_FLAG_DOUBLE_SIDED)
        cullMode = GX_CULL_NONE;
    else
        cullMode = GX_CULL_BACK;
    gxutil_set_vtx_attrs(shape->vtxAttrs);

    if (g_customMaterialFunc != NULL)
    {
        sp20.shape = shape;
        sp20.modelTevs = modelSamplers;
        bvar = g_customMaterialFunc(&sp20);
    }
    else
    {
        build_tev_material((void *)shape, (void *)modelSamplers);
        bvar = 1;
    }

    if (bvar)
    {
        __GXSetDirtyState();
        for (i = 0; i < 2; i++)
        {
            if (shape->dispListFlags & (1 << i))
            {
                if (s_cullMode != cullMode)
                {
                    u32 r3;
                    s_cullMode = cullMode;
                    r3 = (gx->unk204 & ~0xC000) | cullMode << 14;
                    gx->unk204 = r3;
                    func_8008D6BC(r3);
                }
                func_8008FBB0(shape->vtxAttrs, r29, d, shape->dispListSizes[i]);
                d += shape->dispListSizes[i] * 4;
            }
            if (cullMode != 0)
                cullMode = 1;
        }
    }
    return d;
}

void draw_indexed_model(struct GMAModel *model, struct GMAShape *b, struct GMATevLayer *modelTevLayers)
{
    int i;  // r31
    u8 *r30 = b->mtxIndices;
    struct GMAShape *r6 = (void *)((u8 *)b + b->specularColor.asU32);

    s_cullMode = GX_CULL_FRONT;
    for (i = 0; i < model->opaqueShapeCount; i++)
    {
        r6 = draw_shape_reflection_maybe((void *)r30, modelTevLayers, (void *)b, (void *)r6);
        r30 += 0x60;
    }
    for (i = 0; i < model->translucentShapeCount; i++)
    {
        r6 = draw_shape_reflection_maybe((void *)r30, modelTevLayers, (void *)b, (void *)r6);
        r30 += 0x60;
    }
}

void init_tev_material_cache(struct GMAModel *model, struct GMAShape *shape)
{
    s_materialCache.unk0 = 1;
    if (model->flags & (GCMF_SKIN|GCMF_EFFECTIVE))
        shape = (struct GMAShape *)((u8 *)shape + 0x20);
    if (lbl_802F2108 != 0)
        GXLoadTexMtxImm(lbl_802B4E6C, GX_TEXMTX1, GX_MTX3x4);
    else
        GXLoadTexMtxImm(lbl_802B4E9C, GX_TEXMTX1, GX_MTX3x4);
    if (s_zModeUpdateEnable  != gxCache->updateEnable
     || s_zModeCompareFunc   != gxCache->compareFunc
     || s_zModeCompareEnable != gxCache->compareEnable)
    {
        GXSetZMode(s_zModeCompareEnable, s_zModeCompareFunc, s_zModeUpdateEnable);
        gxCache->compareEnable = s_zModeCompareEnable;
        gxCache->compareFunc   = s_zModeCompareFunc;
        gxCache->updateEnable  = s_zModeUpdateEnable;
    }
    if (s_fogEnabled != 0)
        GXSetFog_cached(s_fogType, s_fogStartZ, s_fogEndZ, 0.1f, 20000.0f, s_fogColor);
    else
        GXSetFog_cached(GX_FOG_NONE, 0.0f, 100.0f, 0.1f, 20000.0f, s_fogColor);
    if (shape->flags & (GMA_SHAPE_FLAG_CUSTOM_MAT_AMB_COLOR | GMA_SHAPE_FLAG_SIMPLE_MATERIAL))
        s_materialCache.materialColor = shape->materialColor;
    else
    {
        s_materialCache.materialColor.r = 255;
        s_materialCache.materialColor.g = 255;
        s_materialCache.materialColor.b = 255;
    }
    s_materialCache.materialColor.a = shape->alpha;
    if (shape->flags & GMA_SHAPE_FLAG_CUSTOM_MAT_AMB_COLOR)
        s_materialCache.ambientColor = shape->ambientColor;
    else
    {
        s_materialCache.ambientColor.r = 255;
        s_materialCache.ambientColor.g = 255;
        s_materialCache.ambientColor.b = 255;
    }
    s_materialCache.ambientColor.a = 255;
    set_tev_material_ambient_colors(shape);
    s_materialCache.specularColor.r = shape->specularColor.asColor.r;
    s_materialCache.specularColor.g = shape->specularColor.asColor.g;
    s_materialCache.specularColor.b = shape->specularColor.asColor.b;
    s_materialCache.chanConfig = TEVMATCACHE_CHAN_CONFIG_NULL;
    s_materialCache.colorIn = GX_CC_ZERO;
    s_materialCache.alphaIn = GX_CA_ZERO;
    s_materialCache.num_tev_stages = -1;
    s_materialCache.num_tex_gens = -1;
    s_materialCache.num_ind_stages = -1;
    s_materialCache.postMultiplyTevStageIdx = 16;
    s_materialCache.postAddTevStageIdx = 16;
    if (usePostMultiplyTevStage)
        GXSetTevKColor_cached(GX_KCOLOR2, s_postMultiplyColor);
    if (usePostAddTevStage)
        GXSetTevKColor_cached(GX_KCOLOR3, s_postAddColor);
    s_materialCache.blendSrcFactor = 4;
    s_materialCache.blendDstFactor = 5;
    if (shape->flags & GMA_SHAPE_FLAG_CUSTOM_BLEND_SRC)
        s_materialCache.blendSrcFactor = shape->blendFactors & 0xF;
    if (shape->flags & GMA_SHAPE_FLAG_CUSTOM_BLEND_DST)
        s_materialCache.blendDstFactor = (shape->blendFactors >> 4) & 0xF;
    GXSetBlendMode_cached(GX_BM_BLEND, s_materialCache.blendSrcFactor, s_materialCache.blendDstFactor, GX_LO_CLEAR);
    s_materialCache.tevStageCount = -1;
    s_materialCache.tevLayerFlags[0] = -1;
    s_materialCache.tevLayerFlags[1] = -1;
    s_materialCache.tevLayerFlags[2] = -1;
    s_materialCache.unk38 = -1;
    s_materialCache.tevLayerIdxs[0] = 0xFFFF;
    s_materialCache.tevLayerIdxs[1] = 0xFFFF;
    s_materialCache.tevLayerIdxs[2] = 0xFFFF;
    s_materialCache.unk44 = 0;
    s_materialCache.unk48 = 0;
    s_materialCache.unk4C = 0;
    s_materialCache.viewSpecularColorValid = 0;
    s_materialCache.worldSpecularColorValid = 0;
    mathutil_mtxA_tf_point(&model->boundSphereCenter, &s_materialCache.modelDir_rt_view);
    s_materialCache.modelDir_rt_view.z -= model->boundSphereRadius;
    mathutil_vec_normalize_len(&s_materialCache.modelDir_rt_view);
}

void g_compute_texmtx0(void)
{
    Point3d cameraPos = {0.0f, 0.0f, 0.0f};
    Vec cameraUp = {0.0f, 1.0f, 0.0f};
    Mtx mtx;

    mathutil_mtxA_push();
    MTXLookAt(mtx, &cameraPos, &cameraUp, &s_materialCache.modelDir_rt_view);
    mathutil_mtxA_from_mtx(mtx);
    mathutilData->mtxA[0][3] = 0.5f;
    mathutilData->mtxA[1][0] *= -1.0f;
    mathutilData->mtxA[1][1] *= -1.0f;
    mathutilData->mtxA[1][2] *= -1.0f;
    mathutilData->mtxA[1][3] = 0.5f;
    mathutilData->mtxA[2][0] = 0.0f;
    mathutilData->mtxA[2][1] = 0.0f;
    mathutilData->mtxA[2][2] = 0.0f;
    mathutilData->mtxA[2][3] = 1.0f;
    mathutil_mtxA_scale_s(0.5f);
    GXLoadTexMtxImm(mathutilData->mtxA, GX_PTTEXMTX0, GX_MTX3x4);
    mathutil_mtxA_pop();
}

void g_compute_texmtx1and2(void)
{
    Point3d cameraPos = {0.0f, 0.0f, 0.0f};
    Point3d target;
    Vec sp44;
    Vec cameraUp;
    Mtx mtx;

    mathutil_mtxA_push();
    target = lbl_802B4E60;
    sp44 = s_materialCache.modelDir_rt_view;
    sp44.x *= -0.9f;
    sp44.y *= -0.9f;
    sp44.z *= -0.9f;
    target.x = (sp44.x + target.x) * 0.5f;
    target.y = (sp44.y + target.y) * 0.5f;
    target.z = (sp44.z + target.z) * 0.5f;
    cameraUp.x = target.x - sp44.x;
    cameraUp.y = target.y - sp44.y;
    cameraUp.z = target.z - sp44.z;
    MTXLookAt(mtx, &cameraPos, &cameraUp, &target);
    mathutil_mtxA_from_mtx(mtx);
    mathutilData->mtxA[0][3] = 0.5f;
    mathutilData->mtxA[1][0] *= -1.0f;
    mathutilData->mtxA[1][1] *= -1.0f;
    mathutilData->mtxA[1][2] *= -1.0f;
    mathutilData->mtxA[1][3] = 0.5f;
    mathutilData->mtxA[2][0] = 0.0f;
    mathutilData->mtxA[2][1] = 0.0f;
    mathutilData->mtxA[2][2] = 0.0f;
    mathutilData->mtxA[2][3] = 1.0f;
    mathutil_mtxA_scale_s(0.5f);
    GXLoadTexMtxImm(mathutilData->mtxA, GX_PTTEXMTX1, GX_MTX3x4);
    mathutil_mtxA_from_identity();
    mathutilData->mtxA[0][0] = 0.0f;
    mathutilData->mtxA[0][2] = 0.5f;
    mathutilData->mtxA[0][3] = 0.5f;
    mathutilData->mtxA[1][1] = 0.0f;
    mathutilData->mtxA[2][2] = 0.0f;
    mathutilData->mtxA[2][3] = 1.0f;
    mathutil_mtxA_mult_right(mtx);
    GXLoadTexMtxImm(mathutilData->mtxA, GX_PTTEXMTX2, GX_MTX3x4);
    mathutil_mtxA_pop();
}

void init_some_texture(void)
{
    u8 *imagePtr = unknownTexImg;
    memset(imagePtr,      0xFF, 32);
    memset(imagePtr + 32,    0, 32);
    DCFlushRange(imagePtr, 64);
    GXInitTexObj(
        &unknownTexObj,  // obj
        unknownTexImg,  // image_ptr
        16,  // width
        4,  // height
        GX_TF_I8,  // format
        GX_CLAMP,  // wrap_s
        GX_CLAMP,  // wrap_t
        GX_FALSE);  // mipmap
    GXInitTexObjLOD(
        &unknownTexObj,  // obj
        GX_LINEAR,  // min_filt
        GX_LINEAR,  // mag_filt
        0.0f,  // min_lod
        0.0f,  // max_lod
        0.0f,  // lod_bias
        GX_FALSE,  // bias_clamp
        GX_FALSE,  // do_edge_lod
        GX_ANISO_1);  // max_aniso
}

struct TevStageInfo
{
    s32 tevStage;  // 7C
    GXTexCoordID texCoordId;  // 80
    u32 unk8;  // 84
    GXTexMapID texMapId;  // 88

    u32 tevIndStage;  // 8C
    u32 unk14;  // 90

    u32 unk18;  // 94
    u32 unk1C;  // 98
    u32 unk20;  // 9C
    GXTexCoordID g_texCoordId2;  // A0
    GXTexMapID g_someTexmapId2;
};

struct UnkStruct33
{
    u32 unk0; // 3C
    void *unk4; // 40
    void *unk8; // 44
    struct TevStageInfo unkC;  // 48
};

static inline void material_set_tev_kcolor0(GXColor color)
{
                    if (color.r == 0 && color.g == 0 && color.b == 0)
                    {
                        color.r = 255;
                        color.g = 255;
                        color.b = 255;
                    }
                    //lbl_80090BEC
                    GXSetTevKColor_cached(GX_KCOLOR0, color);
}

static inline void material_set_tev_kcolor1(GXColor color)
{
                    if (color.r == 0 && color.g == 0 && color.b == 0)
                    {
                        color.r = 255;
                        color.g = 255;
                        color.b = 255;
                    }
                    color.r = (float)color.r * g_someColorScaleR;
                    color.g = (float)color.g * g_someColorScaleG;
                    color.b = (float)color.b * g_someColorScaleB;
                    GXSetTevKColor_cached(GX_KCOLOR1, color);
}

static inline void material_set_num_tev_stages(s8 a)
{
    if (s_materialCache.num_tev_stages != a)
    {
        s_materialCache.num_tev_stages = a;
        GXSetNumTevStages_cached(a);
    }
}

static inline void material_set_num_tex_gens(s8 b)
{
    if (s_materialCache.num_tex_gens != b)
    {
        s_materialCache.num_tex_gens = b;
        GXSetNumTexGens(b);
    }
}

static inline void material_set_num_ind_stages(s8 c)
{
    if (s_materialCache.num_ind_stages != c)
    {
        s_materialCache.num_ind_stages = c;
        GXSetNumIndStages(c);
    }
}

#ifdef NONMATCHING
//#if 1
// stack differences
// DOL: 0x8C444
void build_tev_material(struct GMAShape *shape, struct GMATevLayer *modelTevLayers)
{
    struct TevStageInfo tevStageInfo;  // correct
    GXColor materialColor;  // correct
    GXTevColorArg colorIn;
    GXTevAlphaArg alphaIn;
    s32 haveNewColorIn;
    s32 haveNewAlphaIn;
    s32 blendSrcFactor;
    s32 blendDstFactor;

    tevStageInfo.tevStage = GX_TEVSTAGE0;  // 7C
    tevStageInfo.texCoordId = GX_TEXCOORD0;  // 80
    tevStageInfo.unk8 = 0x1E;  // 84
    tevStageInfo.texMapId = GX_TEXMAP0;  // 88
    tevStageInfo.tevIndStage = 0;  // 8C
    haveNewColorIn = 0;
    tevStageInfo.unk14 = 0x40;  // 0x90
    haveNewAlphaIn = 0;
    tevStageInfo.unk18 = 0;  // 94
    tevStageInfo.unk1C = 1;  // 98
    tevStageInfo.unk20 = 0;  // 9C

    tevStageInfo.texMapId = GX_TEXMAP1;  // 88
    tevStageInfo.unk8 = 0x24;  // 84
    tevStageInfo.unk14 = 0x49;  // 90
    tevStageInfo.unk18 = 4;  // 94

    if (s_materialCache.specularColor.r != shape->specularColor.asColor.r
     || s_materialCache.specularColor.g != shape->specularColor.asColor.g
     || s_materialCache.specularColor.b != shape->specularColor.asColor.b)
    {
        s_materialCache.viewSpecularColorValid = 0;
        s_materialCache.worldSpecularColorValid = 0;
        s_materialCache.specularColor.r = shape->specularColor.asColor.r;
        s_materialCache.specularColor.g = shape->specularColor.asColor.g;
        s_materialCache.specularColor.b = shape->specularColor.asColor.b;
    }
    // lbl_800905F8
    if ((shape->flags & GMA_SHAPE_FLAG_UNLIT) == 0)
    {
        GXColor ambientColor;
        int g_someColorChanged = 0;
        if (shape->flags & (GMA_SHAPE_FLAG_SIMPLE_MATERIAL | GMA_SHAPE_FLAG_CUSTOM_MAT_AMB_COLOR))
        {
            materialColor.r = shape->materialColor.r;
            materialColor.g = shape->materialColor.g;
            materialColor.b = shape->materialColor.b;
        }
        else
        {
            materialColor.r = 255;
            materialColor.g = 255;
            materialColor.b = 255;
        }
        //lbl_80090644
        materialColor.a = shape->alpha;
        if (s_materialCache.materialColor.r != materialColor.r
         || s_materialCache.materialColor.g != materialColor.g
         || s_materialCache.materialColor.b != materialColor.b
         || s_materialCache.materialColor.a != materialColor.a)
        {
            g_someColorChanged = 1;
            s_materialCache.materialColor = materialColor;
        }
        //lbl_8009069C
        if (shape->flags & GMA_SHAPE_FLAG_CUSTOM_MAT_AMB_COLOR)
        {
            ambientColor.r = shape->ambientColor.r;
            ambientColor.g = shape->ambientColor.g;
            ambientColor.b = shape->ambientColor.b;
        }
        else
        {
            ambientColor.r = 255;
            ambientColor.g = 255;
            ambientColor.b = 255;
        }
        //lbl_800906C8
        if (s_materialCache.ambientColor.r != ambientColor.r
         || s_materialCache.ambientColor.g != ambientColor.g
         || s_materialCache.ambientColor.b != ambientColor.b)
        {
            g_someColorChanged = 1;
            s_materialCache.ambientColor.r = ambientColor.r;
            s_materialCache.ambientColor.g = ambientColor.g;
            s_materialCache.ambientColor.b = ambientColor.b;
        }
        //lbl_8009070C
        if (g_someColorChanged)
            set_tev_material_ambient_colors(shape);
    }
    //lbl_8009071C
    colorIn = GX_CC_RASC;
    alphaIn = GX_CA_RASA;
    if (shape->flags & GMA_SHAPE_FLAG_UNLIT)
    {
        if (shape->flags & GMA_SHAPE_FLAG_VERT_COLORS)
        {
            if (s_materialCache.chanConfig != TEVMATCACHE_CHAN_CONFIG_UNLIT_VERTCOLORS)
            {
                s_materialCache.chanConfig = TEVMATCACHE_CHAN_CONFIG_UNLIT_VERTCOLORS;
                GXSetChanCtrl(
                    GX_COLOR0A0,  // chan
                    GX_DISABLE,  // enable
                    GX_SRC_VTX,  // amb_src
                    GX_SRC_VTX,  // mat_src
                    GX_LIGHT_NULL,  // light_mask
                    GX_DF_NONE,  // diff_fn
                    GX_AF_NONE);  // attn_fn
            }
            //to lbl_800908D4
        }
        //lbl_8009077C
        else
        {
            s_materialCache.chanConfig = TEVMATCACHE_CHAN_CONFIG_UNLIT;
            if (shape->flags & (GMA_SHAPE_FLAG_CUSTOM_MAT_AMB_COLOR | GMA_SHAPE_FLAG_SIMPLE_MATERIAL))
            {
                materialColor.r = shape->materialColor.r;
                materialColor.g = shape->materialColor.g;
                materialColor.b = shape->materialColor.b;
            }
            else
            {
                materialColor.r = 255;
                materialColor.g = 255;
                materialColor.b = 255;
            }
            //lbl_800907C0
            materialColor.a = (float)shape->alpha * s_materialAlpha;
            //sp38 = sp78;
            GXSetTevColor(GX_TEVREG0, materialColor);
            colorIn = GX_CC_C0;
            alphaIn = GX_CA_A0;
            //to lbl_800908D4
        }
    }
    //lbl_80090814
    else
    {
        if (shape->flags & GMA_SHAPE_FLAG_VERT_COLORS)
        {
            if (s_materialCache.chanConfig != TEVMATCACHE_CHAN_CONFIG_LIGHTS_VERTCOLORS)
            {
                s_materialCache.chanConfig = TEVMATCACHE_CHAN_CONFIG_LIGHTS_VERTCOLORS;
                GXSetChanCtrl(
                    GX_ALPHA0,  // chan
                    GX_DISABLE,  // enable
                    GX_SRC_REG,  // amb_src
                    GX_SRC_VTX,  // mat_src
                    GX_LIGHT_NULL,  // light_mask
                    GX_DF_NONE,  // diff_fn
                    GX_AF_NONE);  // attn_fn
                GXSetChanCtrl(
                    GX_COLOR0,  // chan
                    GX_ENABLE,  // enable
                    GX_SRC_REG,  // amb_src
                    GX_SRC_VTX,  // mat_src
                    s_lightMask,  // light_mask
                    GX_DF_CLAMP,  // diff_fn
                    GX_AF_SPOT);  // attn_fn
            }
            //to lbl_800908D4
        }
        //lbl_8009087C
        else
        {
            if (s_materialCache.chanConfig != TEVMATCACHE_CHAN_CONFIG_LIGHTS)
            {
                s_materialCache.chanConfig = TEVMATCACHE_CHAN_CONFIG_LIGHTS;
                GXSetChanCtrl(
                    GX_ALPHA0,  // chan
                    GX_DISABLE,  // enable
                    GX_SRC_REG,  // amb_src
                    GX_SRC_REG,  // mat_src
                    GX_LIGHT_NULL,  // light_mask
                    GX_DF_NONE,  // diff_fn
                    GX_AF_NONE);  // attn_fn
                GXSetChanCtrl(
                    GX_COLOR0,  // chan
                    GX_ENABLE,  // enable
                    GX_SRC_REG,  // amb_src
                    GX_SRC_REG,  // mat_src
                    s_lightMask,  // light_mask
                    GX_DF_CLAMP,  // diff_fn
                    GX_AF_SPOT);  // attn_fn
            }
        }
    }
    //lbl_800908D4

    if (s_fogEnabled != 0)
    {
        if (shape->flags & GMA_SHAPE_FLAG_NO_FOG)
            GXSetFog_cached(GX_FOG_NONE, 0.0f, 100.0f, 0.1f, 20000.0f, s_fogColor);
        else
            GXSetFog_cached(s_fogType, s_fogStartZ, s_fogEndZ, 0.1f, 20000.0f, s_fogColor);
    }
    //lbl_8009093C
    if (s_materialCache.colorIn != colorIn)
    {
        s_materialCache.colorIn = colorIn;
        haveNewColorIn = 1;
    }
    if (s_materialCache.alphaIn != alphaIn)
    {
        s_materialCache.alphaIn = alphaIn;
        haveNewAlphaIn = 1;
    }
    if (shape->flags & GMA_SHAPE_FLAG_SIMPLE_MATERIAL) // Simple single TEV stage, zero texture material?
    {
        if (s_materialCache.tevStageCount != 0 || haveNewColorIn != 0 || haveNewAlphaIn != 0)
        {
            // Use color/alpha directly from D input with no modifications for this tev stage
            GXSetTevOrder_cached(tevStageInfo.tevStage, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0);
            GXSetTevColorIn_cached(tevStageInfo.tevStage, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, colorIn);
            GXSetTevColorOp_cached(tevStageInfo.tevStage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
            GXSetTevAlphaIn_cached(tevStageInfo.tevStage, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, alphaIn);
            GXSetTevAlphaOp_cached(tevStageInfo.tevStage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
        }
        //lbl_80090A1C
        else
        {
            if (haveNewColorIn != 0) // Impossible
            {
                if (haveNewColorIn != 0)
                    GXSetTevColorIn_cached(tevStageInfo.tevStage, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, colorIn);
                //lbl_80090A40
                if (haveNewAlphaIn != 0)
                    GXSetTevAlphaIn_cached(tevStageInfo.tevStage, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, alphaIn);
            }
        }
        //lbl_80090A60
        s_materialCache.tevLayerFlags[0] = -1;
        tevStageInfo.tevStage++;
        //to lbl_80091020
    }
    //lbl_80090A78
    else
    {
        s32 tevStageCounter = shape->tevLayerCount;
        GXTexGenSrc g_texGenSrc = GX_TG_TEX0;
        // loop:
        // r18 = &a->unk16
        // r17 = &g_tevCache.translucency
        // r15 = &g_tevCache.posXKeyframeCount
        // r24 = &g_tevCache.visibleKeyframes doesn't change
        // r14 = &sp2C doesn't change
        // r25 = &g_tevCache.visibleKeyframeCount  doesn't change
        // maybe indexed instead?
        u16 *tevLayerIdx = shape->tevLayerIdxs;
        u32 *cachedTevLayerFlags = s_materialCache.tevLayerFlags;
        u32 tevLayerFlags;
        u16 *cachedTevLayerIdx = s_materialCache.tevLayerIdxs;
        while (tevStageCounter > 0)
        {
            struct GMATevLayer *tevLayer = &modelTevLayers[*tevLayerIdx];
            tevLayerFlags = tevLayer->flags;
            tevLayerFlags &= (GMA_TEV_LAYER_FLAG_TYPE_ALPHA_BLEND | GMA_TEV_LAYER_FLAG_TYPE3 | GMA_TEV_LAYER_FLAG_TYPE_VIEW_SPECULAR |
                      GMA_TEV_LAYER_FLAG_TYPE_WORLD_SPECULAR);
            if (*cachedTevLayerFlags != tevLayerFlags)
                break;
            if (*cachedTevLayerIdx != *tevLayerIdx || (tevLayer->flags & GMA_TEV_LAYER_FLAG_UNK16))
            {
                GXLoadTexObj_cached(tevLayer->texObj, tevStageInfo.texMapId);
                *cachedTevLayerIdx = *tevLayerIdx;
            }
            //lbl_80090B00
            if (tevLayerFlags == 0)
            {
                if (haveNewColorIn != 0)
                {
                    build_diffuse_layer_cached(&tevStageInfo, colorIn, alphaIn);
                    haveNewColorIn = 0;
                }
                diffuse_layer_next(&tevStageInfo);
                //to lbl_80090D3C
            }
            //lbl_80090B30
            else if (tevLayerFlags & GMA_TEV_LAYER_FLAG_TYPE_ALPHA_BLEND)
            {
                if (haveNewColorIn != 0)
                {
                    build_alpha_blend_layer_cached(&tevStageInfo, colorIn, alphaIn);
                    haveNewColorIn = 0;
                }
                alpha_blend_layer_next(&tevStageInfo);
                //to lbl_80090D3C
            }
            else if (tevLayerFlags & GMA_TEV_LAYER_FLAG_TYPE3)
            {
                if (haveNewColorIn != 0)
                {
                    build_unk3_layer_cached(&tevStageInfo, colorIn, alphaIn);
                    haveNewColorIn = 0;
                }
                unk3_layer_next(&tevStageInfo);
                //to lbl_80090D3C
            }
            //lbl_80090B98
            else if (tevLayerFlags & GMA_TEV_LAYER_FLAG_TYPE_VIEW_SPECULAR)
            {
                if (s_materialCache.viewSpecularColorValid == 0)
                {
                    /*
                    GXColor sp28 = g_tevCache.unk18;
                    if (sp28.r != 0 || sp28.g != 0 || sp28.b != 0)
                    {
                        sp28.r = 255;
                        sp28.g = 255;
                        sp28.b = 255;
                    }
                    //lbl_80090BEC
                    GXSetTevKColor_cached(0, sp28);
                    g_tevCache.unk50 = 1;
                    */
                    // 40 -> 44 (g_tevCache.unk18 temp)
                    // r14 44 -> 28 (arg to GXSetTevKColor_cached)
                    material_set_tev_kcolor0(s_materialCache.specularColor);
                    s_materialCache.viewSpecularColorValid = 1;
                }
                //lbl_80090C08
                if (haveNewColorIn != 0)
                {
                    build_view_specular_layer_cached(&tevStageInfo, colorIn, alphaIn);
                    haveNewColorIn = 0;
                }
                view_specular_layer_next(&tevStageInfo);
                //to lbl_80090D3C
            }
            //lbl_80090C30
            else // (flags & GMA_TEV_LAYER_FLAG_TYPE_WORLD_SPECULAR)
            {
                if (s_materialCache.worldSpecularColorValid == 0)
                {
                    /*
                    GXColor sp20 = g_tevCache.unk18;
                    if (sp20.r == 0 && sp20.g == 0 && sp20.b == 0)
                    {
                        sp20.r = 255;
                        sp20.g = 255;
                        sp20.b = 255;
                    }
                    sp20.r = (float)sp20.r * g_someColorScaleR;
                    sp20.g = (float)sp20.g * g_someColorScaleG;
                    sp20.b = (float)sp20.b * g_someColorScaleB;
                    GXSetTevKColor_cached(1, sp20);
                    g_tevCache.unk54 = 1;
                    */
                    // 32 -> 40 (g_tevCache.unk18 temp)
                    // r27 36 -> 24 (arg to GXSetTevKColor_cached)
                    material_set_tev_kcolor1(s_materialCache.specularColor);
                    s_materialCache.worldSpecularColorValid = 1;
                }
                //lbl_80090D18
                if (haveNewColorIn != 0)
                {
                    build_world_specular_layer_cached(&tevStageInfo, colorIn, alphaIn);
                    haveNewColorIn = 0;
                }
                world_specular_layer_next(&tevStageInfo);
            }
            //lbl_80090D3C
            tevStageInfo.texMapId++;
            g_texGenSrc++;
            tevStageCounter--;
            tevLayerIdx++;
            cachedTevLayerFlags++;
            cachedTevLayerIdx++;
            colorIn = GX_CC_CPREV;
            alphaIn = GX_CA_APREV;
        }
        //lbl_80090D70
        while (tevStageCounter > 0)
        {
            struct GMATevLayer *tevLayer = &modelTevLayers[*tevLayerIdx];  // r4
            u32 tevLayerFlags = tevLayer->flags; // actually, r27
            tevLayerFlags &= (GMA_TEV_LAYER_FLAG_TYPE_ALPHA_BLEND | GMA_TEV_LAYER_FLAG_TYPE3 | GMA_TEV_LAYER_FLAG_TYPE_VIEW_SPECULAR |
                      GMA_TEV_LAYER_FLAG_TYPE_WORLD_SPECULAR);
            *cachedTevLayerFlags = tevLayerFlags;
            if (*cachedTevLayerIdx != *tevLayerIdx || (tevLayer->flags & GMA_TEV_LAYER_FLAG_UNK16))
            {
                GXLoadTexObj_cached(tevLayer->texObj, tevStageInfo.texMapId);
                *cachedTevLayerIdx = *tevLayerIdx;
            }
            if (tevLayerFlags == 0)
            {
                build_diffuse_layer_uncached(&tevStageInfo, colorIn, alphaIn, g_texGenSrc);
                diffuse_layer_next(&tevStageInfo);
            }
            else if (tevLayerFlags & GMA_TEV_LAYER_FLAG_TYPE_ALPHA_BLEND)
            {
                build_alpha_blend_layer_uncached(&tevStageInfo, colorIn, alphaIn, g_texGenSrc);
                alpha_blend_layer_next(&tevStageInfo);
            }
            // Likely bug, meant to test for next layer type. In the cached version (previous while
            // loop) it does.
            else if (tevLayerFlags & GMA_TEV_LAYER_FLAG_TYPE_ALPHA_BLEND)
            {
                build_unk3_layer_uncached(&tevStageInfo, colorIn, alphaIn, g_texGenSrc);
                unk3_layer_next(&tevStageInfo);
            }
            //lbl_80090E54
            else if (tevLayerFlags & GMA_TEV_LAYER_FLAG_TYPE_VIEW_SPECULAR)
            {
                if (s_materialCache.viewSpecularColorValid == 0)
                {
                    /*
                    GXColor sp28 = g_tevCache.unk18;
                    if (sp28.r != 0 || sp28.g != 0 || sp28.b != 0)
                    {
                        sp28.r = 255;
                        sp28.g = 255;
                        sp28.b = 255;
                    }
                    GXSetTevKColor_cached(0, sp28);
                    g_tevCache.unk50 = 1;
                    */
                    // 24 -> 36 (g_tevCache.unk18 temp)
                    // 28 -> 20 (arg to GXSetTevKColor_cached)
                    material_set_tev_kcolor0(s_materialCache.specularColor);
                    s_materialCache.viewSpecularColorValid = 1;
                }
                build_view_specular_layer_uncached(&tevStageInfo, colorIn, alphaIn, g_texGenSrc);
                view_specular_layer_next(&tevStageInfo);
            }
            else
            {
                if (s_materialCache.worldSpecularColorValid == 0)
                {
                    /*
                    GXColor sp20 = g_tevCache.unk18;
                    if (sp20.r == 0 && sp20.g == 0 && sp20.b == 0)
                    {
                        sp20.r = 255;
                        sp20.g = 255;
                        sp20.b = 255;
                    }
                    sp20.r = (float)sp20.r * g_someColorScaleR;
                    sp20.g = (float)sp20.g * g_someColorScaleG;
                    sp20.b = (float)sp20.b * g_someColorScaleB;
                    GXSetTevKColor_cached(1, sp20);
                    g_tevCache.unk54 = 1;
                    */
                    // 16 -> 32 (g_tevCache.unk18 temp)
                    // r21 20 -> 16 (arg to GXSetTevKColor_cached)
                    material_set_tev_kcolor1(s_materialCache.specularColor);
                    s_materialCache.worldSpecularColorValid = 1;
                }
                build_world_specular_layer_uncached(&tevStageInfo, colorIn, alphaIn, g_texGenSrc);
                world_specular_layer_next(&tevStageInfo);
            }
            tevStageInfo.texMapId++;
            g_texGenSrc++;
            tevStageCounter--;
            tevLayerIdx++;
            cachedTevLayerFlags++;
            cachedTevLayerIdx++;
            colorIn = GX_CC_CPREV;
            alphaIn = GX_CA_APREV;
        }
        *cachedTevLayerFlags = -1;
    }
    //lbl_80091020
    if (lbl_802F20EC != NULL)
    {
        struct UnkStruct33 sp3C;
        sp3C.unk0 = s_materialCache.unk0;
        sp3C.unk4 = shape;
        sp3C.unk8 = modelTevLayers;
        sp3C.unkC = tevStageInfo;
        lbl_802F20EC((void *)&sp3C);
        tevStageInfo = sp3C.unkC;
    }
    //lbl_800910F8
    if (usePostMultiplyTevStage)
    {
        if (s_materialCache.postMultiplyTevStageIdx != tevStageInfo.tevStage)
        {
            s_materialCache.postMultiplyTevStageIdx = tevStageInfo.tevStage;
            build_post_multiply_tev_stage(tevStageInfo.tevStage);
        }
        tevStageInfo.tevStage++;
    }
    //lbl_8009112C
    if (usePostAddTevStage)
    {
        if (s_materialCache.postAddTevStageIdx != tevStageInfo.tevStage)
        {
            // Bug?
            s_materialCache.postMultiplyTevStageIdx = tevStageInfo.tevStage;
            build_post_add_tev_stage(tevStageInfo.tevStage);
        }
        tevStageInfo.tevStage++;
    }
    //lbl_8009115C
    GXSetNumChans(1);
    material_set_num_tev_stages(tevStageInfo.tevStage);
    material_set_num_tex_gens(tevStageInfo.texCoordId);
    material_set_num_ind_stages(tevStageInfo.tevIndStage);
    //lbl_800911D0
    blendSrcFactor = GX_BL_SRCALPHA;
    blendDstFactor = GX_BL_INVSRCALPHA;
    if (shape->flags & GMA_SHAPE_FLAG_CUSTOM_BLEND_SRC)
        blendSrcFactor = shape->blendFactors & 0xF;
    if (shape->flags & GMA_SHAPE_FLAG_CUSTOM_BLEND_DST)
        blendDstFactor = (shape->blendFactors >> 4) & 0xF;
    if (s_materialCache.blendSrcFactor != blendSrcFactor || s_materialCache.blendDstFactor != blendDstFactor)
    {
        GXSetBlendMode_cached(GX_BM_BLEND, blendSrcFactor, blendDstFactor, GX_LO_CLEAR);
        s_materialCache.blendSrcFactor = blendSrcFactor;
        s_materialCache.blendDstFactor = blendDstFactor;
    }
    //lbl_8009123C
    s_materialCache.unk0 = 0;
    if (shape->flags & GMA_SHAPE_FLAG_SIMPLE_MATERIAL)
        s_materialCache.tevStageCount = 0;
    else
        s_materialCache.tevStageCount = shape->tevLayerCount;
}
#else
asm void build_tev_material(struct GMAShape *a, struct GMATevLayer *b)
{
#define _SDA_BASE_ 0
#define _SDA2_BASE_ 0
    nofralloc
#include "../asm/nonmatchings/build_tev_material.s"
}
#endif

// Optionally multiply a color/alpha with tev end result
void build_post_multiply_tev_stage(GXTevStageID tevStage)
{
    GXSetTevKColorSel_cached(tevStage, GX_TEV_KCSEL_K2);
    GXSetTevKAlphaSel_cached(tevStage, GX_TEV_KASEL_K2_A);
    GXSetTevDirect(tevStage);
    GXSetTevOrder_cached(tevStage, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0);
    GXSetTevColorIn_cached(tevStage, GX_CC_ZERO, GX_CC_CPREV, GX_CC_KONST, GX_CC_ZERO);
    GXSetTevColorOp_cached(tevStage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevAlphaIn_cached(tevStage, GX_CA_ZERO, GX_CA_APREV, GX_CA_KONST, GX_CA_ZERO);
    GXSetTevAlphaOp_cached(tevStage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
}

// Optionally add a color/alpha to tev end result
void build_post_add_tev_stage(GXTevStageID tevStage)
{
    GXSetTevKColorSel_cached(tevStage, GX_TEV_KCSEL_K3);
    GXSetTevKAlphaSel_cached(tevStage, GX_TEV_KASEL_K3_A);
    GXSetTevDirect(tevStage);
    GXSetTevOrder_cached(tevStage, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0);
    GXSetTevColorIn_cached(tevStage, GX_CC_CPREV, GX_CC_ZERO, GX_CC_ZERO, GX_CC_KONST);
    GXSetTevColorOp_cached(tevStage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevAlphaIn_cached(tevStage, GX_CA_APREV, GX_CA_ZERO, GX_CA_ZERO, GX_CA_KONST);
    GXSetTevAlphaOp_cached(tevStage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
}

void build_diffuse_layer_uncached(struct TevStageInfo *info, GXTevColorArg colorIn, GXTevAlphaArg alphaIn, GXTexGenSrc texGenSrc)
{
    GXSetTevDirect(info->tevStage);
    GXSetTevSwapMode_cached(info->tevStage, GX_TEV_SWAP0, GX_TEV_SWAP0);
    GXSetTexCoordGen(info->texCoordId, GX_TG_MTX2x4, texGenSrc, GX_TEXMTX1);
    GXSetTevOrder_cached(info->tevStage, info->texCoordId, info->texMapId, GX_COLOR0A0);
    GXSetTevColorIn_cached(info->tevStage, GX_CC_ZERO, GX_CC_TEXC, colorIn, GX_CC_ZERO);
    GXSetTevColorOp_cached(info->tevStage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevAlphaIn_cached(info->tevStage, GX_CA_ZERO, GX_CA_TEXA, alphaIn, GX_CA_ZERO);
    GXSetTevAlphaOp_cached(info->tevStage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
}

void build_diffuse_layer_cached(struct TevStageInfo *info, GXTevColorArg colorIn, GXTevAlphaArg alphaIn)
{
    GXSetTevColorIn_cached(info->tevStage, GX_CC_ZERO, GX_CC_TEXC, colorIn, GX_CC_ZERO);
    GXSetTevAlphaIn_cached(info->tevStage, GX_CA_ZERO, GX_CA_TEXA, alphaIn, GX_CA_ZERO);
}

void diffuse_layer_next(struct TevStageInfo *a)
{
    a->tevStage++;
    a->texCoordId++;
}

void build_alpha_blend_layer_uncached(struct TevStageInfo *info, GXTevColorArg colorIn, GXTevAlphaArg alphaIn, GXTexGenSrc texGenSrc)
{
    GXSetTevDirect(info->tevStage);
    GXSetTexCoordGen(info->texCoordId, GX_TG_MTX2x4, texGenSrc, GX_TEXMTX1);
    GXSetTevOrder_cached(info->tevStage, info->texCoordId, info->texMapId, GX_COLOR0A0);
    GXSetTevSwapMode_cached(info->tevStage, GX_TEV_SWAP0, GX_TEV_SWAP1);
    GXSetTevColorIn_cached(info->tevStage, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, colorIn);
    GXSetTevColorOp_cached(info->tevStage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevAlphaIn_cached(info->tevStage, GX_CA_ZERO, GX_CA_TEXA, alphaIn, GX_CA_ZERO);
    GXSetTevAlphaOp_cached(info->tevStage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
}

void build_alpha_blend_layer_cached(struct TevStageInfo *info, GXTevColorArg colorIn, GXTevAlphaArg alphaIn)
{
    GXSetTevColorIn_cached(info->tevStage, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, colorIn);
    GXSetTevAlphaIn_cached(info->tevStage, GX_CA_ZERO, GX_CA_TEXA, alphaIn, GX_CA_ZERO);
}

void alpha_blend_layer_next(struct TevStageInfo *a)
{
    a->tevStage++;
    a->texCoordId++;
}

void build_view_specular_layer_uncached(struct TevStageInfo *info, GXTevColorArg colorIn, GXTevAlphaArg alphaIn, u32 d)
{
    GXSetTevDirect(info->tevStage);
    GXSetTevSwapMode_cached(info->tevStage, GX_TEV_SWAP0, GX_TEV_SWAP0);
    if (s_materialCache.unk44 == 0)
    {
        mathutil_mtxA_push();
        mathutilData->mtxA[0][3] = 0.0f;
        mathutilData->mtxA[1][3] = 0.0f;
        mathutilData->mtxA[2][3] = 0.0f;
        GXLoadTexMtxImm(mathutilData->mtxA, GX_TEXMTX0, GX_MTX3x4);
        mathutil_mtxA_pop();
        s_materialCache.unk44 = 1;
    }
    if (s_materialCache.unk48 == 0)
    {
        g_compute_texmtx0();
        s_materialCache.unk48 = 1;
    }
    GXSetTevKColorSel_cached(info->tevStage, GX_TEV_KCSEL_K0);
    GXSetTexCoordGen2(info->texCoordId, GX_TG_MTX3x4, GX_TG_NRM, GX_TEXMTX0, GX_TRUE, GX_PTTEXMTX0);
    GXSetTevOrder_cached(info->tevStage, info->texCoordId, info->texMapId, GX_COLOR0A0);
    GXSetTevColorIn_cached(info->tevStage, GX_CC_ZERO, GX_CC_TEXC, GX_CC_KONST, colorIn);
    GXSetTevColorOp_cached(info->tevStage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevAlphaIn_cached(info->tevStage, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, alphaIn);
    GXSetTevAlphaOp_cached(info->tevStage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
}

void build_view_specular_layer_cached(struct TevStageInfo *info, GXTevColorArg colorIn, GXTevAlphaArg alphaIn)
{
    GXSetTevColorIn_cached(info->tevStage, GX_CC_ZERO, GX_CC_TEXC, GX_CC_KONST, colorIn);
    GXSetTevAlphaIn_cached(info->tevStage, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, alphaIn);
}

void view_specular_layer_next(struct TevStageInfo *a)
{
    a->tevStage++;
    a->texCoordId++;
}

void build_world_specular_layer_uncached(struct TevStageInfo *info, GXTevColorArg colorIn, GXTevAlphaArg alphaIn, u32 d)
{
    u32 tevStage;

    if (s_materialCache.unk44 == 0)
    {
        mathutil_mtxA_push();
        mathutilData->mtxA[0][3] = 0.0f;
        mathutilData->mtxA[1][3] = 0.0f;
        mathutilData->mtxA[2][3] = 0.0f;
        GXLoadTexMtxImm(mathutilData->mtxA, GX_TEXMTX0, GX_MTX3x4);
        mathutil_mtxA_pop();
        s_materialCache.unk44 = 1;
    }
    if (s_materialCache.unk4C == 0)
    {
        GXLoadTexObj_cached(&unknownTexObj, GX_TEXMAP0);
        g_compute_texmtx1and2();
        s_materialCache.unk4C = 1;
    }
    // unrolled loop?
    tevStage = info->tevStage;

    GXSetTevDirect(tevStage);
    GXSetTevSwapMode_cached(info->tevStage, GX_TEV_SWAP0, GX_TEV_SWAP0);
    GXSetTevKColorSel_cached(tevStage, GX_TEV_KCSEL_K1);
    GXSetTexCoordGen2(info->texCoordId, GX_TG_MTX3x4, GX_TG_NRM, GX_TEXMTX0, GX_TRUE, GX_PTTEXMTX2);
    GXSetTevOrder_cached(tevStage, info->texCoordId, GX_TEXMAP0, GX_COLOR0A0);
    GXSetTevColorIn_cached(tevStage, GX_CC_ZERO, GX_CC_TEXC, GX_CC_KONST, GX_CC_ZERO);
    GXSetTevColorOp_cached(tevStage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVREG2);
    GXSetTevAlphaIn_cached(tevStage, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, alphaIn);
    GXSetTevAlphaOp_cached(tevStage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVREG2);

    GXSetTevDirect(tevStage + 1);
    GXSetTevSwapMode_cached(info->tevStage, GX_TEV_SWAP0, GX_TEV_SWAP0);
    GXSetTexCoordGen2(info->texCoordId + 1, GX_TG_MTX3x4, GX_TG_NRM, GX_TEXMTX0, GX_TRUE, GX_PTTEXMTX1);
    GXSetTevOrder_cached(tevStage + 1, info->texCoordId + 1, info->texMapId, GX_COLOR0A0);
    GXSetTevColorIn_cached(tevStage + 1, GX_CC_ZERO, GX_CC_TEXC, GX_CC_C2, colorIn);
    GXSetTevColorOp_cached(tevStage + 1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevAlphaIn_cached(tevStage + 1, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, alphaIn);
    GXSetTevAlphaOp_cached(tevStage + 1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
}

void build_world_specular_layer_cached(struct TevStageInfo *info, GXTevColorArg colorIn, GXTevAlphaArg alphaIn)
{
    GXSetTevColorIn_cached(info->tevStage + 1, GX_CC_ZERO, GX_CC_TEXC, GX_CC_C2, colorIn);
    GXSetTevAlphaIn_cached(info->tevStage + 1, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, alphaIn);
}

void world_specular_layer_next(struct TevStageInfo *info)
{
    info->tevStage += 2;
    info->texCoordId += 2;
}

// Type 3 seems unused/unreachable

void build_unk3_layer_uncached(struct TevStageInfo *info, GXTevColorArg colorIn, GXTevAlphaArg alphaIn, GXTexGenSrc texGenSrc)
{
    GXSetTevDirect(info->tevStage);
    GXSetTexCoordGen(info->texCoordId, GX_TG_MTX2x4, texGenSrc, GX_TEXMTX1);
    GXSetTevOrder_cached(info->tevStage, info->texCoordId, info->texMapId, GX_COLOR0A0);
    GXSetTevColorIn_cached(info->tevStage, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, colorIn);
    GXSetTevColorOp_cached(info->tevStage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevAlphaIn_cached(info->tevStage, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, alphaIn);
    GXSetTevAlphaOp_cached(info->tevStage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    info->unk20 = 1;
    info->g_texCoordId2 = info->texCoordId;
    info->g_someTexmapId2 = info->texMapId;
}

void build_unk3_layer_cached(struct TevStageInfo *info, GXTevColorArg colorIn, GXTevAlphaArg alphaIn)
{
    GXSetTevColorIn_cached(info->tevStage, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, colorIn);
    GXSetTevAlphaIn_cached(info->tevStage, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, alphaIn);
}

void unk3_layer_next(struct TevStageInfo *info)
{
    info->tevStage++;
    info->texCoordId++;
}
