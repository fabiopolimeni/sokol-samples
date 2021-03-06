//------------------------------------------------------------------------------
//  bufferoffsets-sapp.c
//  Render separate geometries in vertex- and index-buffers with
//  buffer offsets.
//------------------------------------------------------------------------------
#include "sokol_app.h"
#include "sokol_gfx.h"
#include "dbgui/dbgui.h"
#include "bufferoffsets-sapp.glsl.h"

typedef struct {
    float x, y, r, g, b;
} vertex_t;

static sg_pass_action pass_action = {
    .colors = {
        [0] = { .action=SG_ACTION_CLEAR, .val = { 0.5f, 0.5f, 1.0f, 1.0f } }
    }
};
static sg_pipeline pip;
static sg_bindings bind;

void init(void) {
    sg_setup(&(sg_desc){
        .gl_force_gles2 = sapp_gles2(),
        .mtl_device = sapp_metal_get_device(),
        .mtl_renderpass_descriptor_cb = sapp_metal_get_renderpass_descriptor,
        .mtl_drawable_cb = sapp_metal_get_drawable,
        .d3d11_device = sapp_d3d11_get_device(),
        .d3d11_device_context = sapp_d3d11_get_device_context(),
        .d3d11_render_target_view_cb = sapp_d3d11_get_render_target_view,
        .d3d11_depth_stencil_view_cb = sapp_d3d11_get_depth_stencil_view
    });
    __dbgui_setup(1);

    /* a 2D triangle and quad in 1 vertex buffer and 1 index buffer */
    vertex_t vertices[7] = {
        /* triangle */
        {  0.0f,   0.55f,  1.0f, 0.0f, 0.0f },
        {  0.25f,  0.05f,  0.0f, 1.0f, 0.0f },
        { -0.25f,  0.05f,  0.0f, 0.0f, 1.0f },

        /* quad */
        { -0.25f, -0.05f,  0.0f, 0.0f, 1.0f },
        {  0.25f, -0.05f,  0.0f, 1.0f, 0.0f },
        {  0.25f, -0.55f,  1.0f, 0.0f, 0.0f },
        { -0.25f, -0.55f,  1.0f, 1.0f, 0.0f }        
    };
    uint16_t indices[9] = {
        0, 1, 2,
        0, 1, 2, 0, 2, 3
    };
    bind.vertex_buffers[0] = sg_make_buffer(&(sg_buffer_desc){
        .size = sizeof(vertices),
        .content = vertices,
    });
    bind.index_buffer = sg_make_buffer(&(sg_buffer_desc){
        .type = SG_BUFFERTYPE_INDEXBUFFER,
        .size = sizeof(indices),
        .content = indices,
    });

    /* a shader and pipeline to render 2D shapes */
    pip = sg_make_pipeline(&(sg_pipeline_desc){
        .shader = sg_make_shader(bufferoffsets_shader_desc()),
        .index_type = SG_INDEXTYPE_UINT16,
        .layout = {
            .attrs = {
                [0].format=SG_VERTEXFORMAT_FLOAT2,
                [1].format=SG_VERTEXFORMAT_FLOAT3
            }
        }
    });
}

void frame(void) {
    sg_begin_default_pass(&pass_action, sapp_width(), sapp_height());
    sg_apply_pipeline(pip);
    /* render the triangle */
    bind.vertex_buffer_offsets[0] = 0;
    bind.index_buffer_offset = 0;
    sg_apply_bindings(&bind);
    sg_draw(0, 3, 1);
    /* render the quad */
    bind.vertex_buffer_offsets[0] = 3 * sizeof(vertex_t);
    bind.index_buffer_offset = 3 * sizeof(uint16_t);
    sg_apply_bindings(&bind);
    sg_draw(0, 6, 1);
    __dbgui_draw();
    sg_end_pass();
    sg_commit();
}

void cleanup(void) {
    __dbgui_shutdown();
    sg_shutdown();
}

sapp_desc sokol_main(int argc, char* argv[]) {
    return (sapp_desc){
        .init_cb = init,
        .frame_cb = frame,
        .cleanup_cb = cleanup,
        .event_cb = __dbgui_event,
        .width = 800,
        .height = 600,
        .gl_force_gles2 = true,
        .window_title = "Buffer Offsets (sokol-app)",
    };
}

