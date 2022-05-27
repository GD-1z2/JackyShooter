const game_state = @import("game_state.zig");
const gl = @import("zgl");
const glfw = @import("mach-glfw");
const zlm = @import("zlm");
const zlm32 = zlm.SpecializeOn(f32);
const shader = @import("shader.zig");
const std = @import("std");
const tex = @import("texture.zig");

pub fn main() anyerror!void {
    try glfw.init(.{});
    defer glfw.terminate();

    const window = try glfw.Window.create(1280, 720, "Jacky Shooter", null, null, .{
        .context_version_major = 3,
        .context_version_minor = 3,
        .opengl_profile = .opengl_core_profile,
    });

    try glfw.makeContextCurrent(window);
    try glfw.swapInterval(0);

    window.setFramebufferSizeCallback(onResize);

    var gp = std.heap.GeneralPurposeAllocator(.{ .safety = true }){};
    defer _ = gp.deinit();
    const allocator = gp.allocator();

    var game = game_state.Game{};
    {
        const vert_shader = "assets/shaders/default.vert";
        const frag_shader = "assets/shaders/default.frag";
        game.default_shader = try shader.buildProgram(vert_shader, frag_shader, allocator);
    }

    const vertices = ([_]f32{
        -0.5, -0.5, -0.5, 0.0, 0.0,
        0.5,  -0.5, -0.5, 1.0, 0.0,
        0.5,  0.5,  -0.5, 1.0, 1.0,
        0.5,  0.5,  -0.5, 1.0, 1.0,
        -0.5, 0.5,  -0.5, 0.0, 1.0,
        -0.5, -0.5, -0.5, 0.0, 0.0,

        -0.5, -0.5, 0.5,  0.0, 0.0,
        0.5,  -0.5, 0.5,  1.0, 0.0,
        0.5,  0.5,  0.5,  1.0, 1.0,
        0.5,  0.5,  0.5,  1.0, 1.0,
        -0.5, 0.5,  0.5,  0.0, 1.0,
        -0.5, -0.5, 0.5,  0.0, 0.0,

        -0.5, 0.5,  0.5,  1.0, 0.0,
        -0.5, 0.5,  -0.5, 1.0, 1.0,
        -0.5, -0.5, -0.5, 0.0, 1.0,
        -0.5, -0.5, -0.5, 0.0, 1.0,
        -0.5, -0.5, 0.5,  0.0, 0.0,
        -0.5, 0.5,  0.5,  1.0, 0.0,

        0.5,  0.5,  0.5,  1.0, 0.0,
        0.5,  0.5,  -0.5, 1.0, 1.0,
        0.5,  -0.5, -0.5, 0.0, 1.0,
        0.5,  -0.5, -0.5, 0.0, 1.0,
        0.5,  -0.5, 0.5,  0.0, 0.0,
        0.5,  0.5,  0.5,  1.0, 0.0,

        -0.5, -0.5, -0.5, 0.0, 1.0,
        0.5,  -0.5, -0.5, 1.0, 1.0,
        0.5,  -0.5, 0.5,  1.0, 0.0,
        0.5,  -0.5, 0.5,  1.0, 0.0,
        -0.5, -0.5, 0.5,  0.0, 0.0,
        -0.5, -0.5, -0.5, 0.0, 1.0,

        -0.5, 0.5,  -0.5, 0.0, 1.0,
        0.5,  0.5,  -0.5, 1.0, 1.0,
        0.5,  0.5,  0.5,  1.0, 0.0,
        0.5,  0.5,  0.5,  1.0, 0.0,
        -0.5, 0.5,  0.5,  0.0, 0.0,
        -0.5, 0.5,  -0.5, 0.0, 1.0,
    })[0..];

    const vao = gl.VertexArray.gen();
    const vbo = gl.Buffer.gen();

    vao.bind();

    vbo.bind(.array_buffer);
    vbo.data(f32, vertices, .static_draw);

    gl.vertexAttribPointer(0, 3, .float, false, 5 * @sizeOf(f32), 0);
    vao.enableVertexAttribute(0);

    gl.vertexAttribPointer(1, 3, .float, false, 5 * @sizeOf(f32), 3 * @sizeOf(f32));
    vao.enableVertexAttribute(1);

    const tx = tex.loadTexture("assets/textures/container.jpg", false);
    defer gl.Texture.delete(tx);

    gl.clearColor(0.2, 0.3, 0.3, 1.0);
    gl.useProgram(game.default_shader);
    gl.enable(.depth_test);

    {
        game.default_shader.uniform1i(game.default_shader.uniformLocation("texture1"), 0);

        const proj = zlm32.Mat4.createPerspective(45.0 * std.math.pi / 180.0, 1280.0 / 720.0, 0.1, 100.0);
        const view = zlm32.Mat4.createTranslationXYZ(1, -1, 5);

        game.default_shader.uniformMatrix4(
            game.default_shader.uniformLocation("projection"),
            false,
            ([_][4][4]f32{proj.fields})[0..],
        );
        game.default_shader.uniformMatrix4(
            game.default_shader.uniformLocation("view"),
            false,
            ([_][4][4]f32{view.fields})[0..],
        );
        game.default_shader.uniformMatrix4(
            game.default_shader.uniformLocation("model"),
            false,
            ([_][4][4]f32{zlm32.Mat4.identity.fields})[0..],
        );
    }

    var timer: i128 = 0;

    while (!window.shouldClose()) {
        std.debug.print("FPS : {}\n", .{@floatToInt(
            i32,
            1 / (@intToFloat(f128, std.time.nanoTimestamp() - timer) / 1e9),
        )});
        timer = std.time.nanoTimestamp();

        if (.release != window.getKey(.escape))
            window.setShouldClose(true);

        gl.clear(.{ .color = true, .depth = true });

        gl.drawArrays(.triangles, 0, 36);

        try window.swapBuffers();
        try glfw.pollEvents();
    }
}

fn onResize(_: glfw.Window, width: u32, height: u32) void {
    gl.viewport(0, 0, width, height);
}
