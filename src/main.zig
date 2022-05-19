const std = @import("std");
// const glfw = @import("../lib/mach-glfw/src/main.zig");
const glfw = @import("mach-glfw");
const gl = @import("zgl");

pub fn main() anyerror!void {
    try glfw.init(.{});
    defer glfw.terminate();

    const window = try glfw.Window.create(1280, 720, "Jacky Shooter", null, null, .{
        .context_version_major = 3,
        .context_version_minor = 3,
        .opengl_profile = glfw.Window.Hints.OpenGLProfile.opengl_core_profile,
    });

    try glfw.makeContextCurrent(window);

    gl.clearColor(0.2, 0.3, 0.3, 1.0);

    while (!window.shouldClose()) {
        gl.clear(.{.color = true});
        try window.swapBuffers();
        try glfw.pollEvents();
    }
}
