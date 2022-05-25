const files = @import("files.zig");
const gl = @import("zgl");
const std = @import("std");

pub fn buildShader(path: []const u8, shader_type: gl.ShaderType, allocator: std.mem.Allocator) !gl.Shader {
    const shader = gl.Shader.create(shader_type);

    const source = try files.readFile(path, allocator);
    defer allocator.free(source);

    gl.Shader.source(shader, 1, &source);
    gl.Shader.compile(shader);

    const compile_log = try gl.Shader.getCompileLog(shader, allocator);
    defer allocator.free(compile_log);
    if (compile_log.len > 0) {
        std.log.err("Shader compile log: {s}\n", .{compile_log});
        return error.ShaderCompileError;
    }

    return shader;
}

pub fn buildProgram(vertex_path: []const u8, fragment_path: []const u8, allocator: std.mem.Allocator) !gl.Program {
    const vertex_shader = try buildShader(vertex_path, .vertex, allocator);
    const fragment_shader = try buildShader(fragment_path, .fragment, allocator);

    const program = gl.Program.create();
    gl.Program.attach(program, vertex_shader);
    gl.Program.attach(program, fragment_shader);
    gl.Program.link(program);

    const link_log = try gl.Program.getCompileLog(program, allocator);
    defer allocator.free(link_log);
    if (link_log.len > 0) {
        std.log.err("Shader program link log: {s}\n", .{link_log});
        return error.ProgramLinkError;
    }

    return program;
}
