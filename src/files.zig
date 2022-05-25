const std = @import("std");

pub fn readFile(rel_path: []const u8, allocator: std.mem.Allocator) ![]u8 {
    var path_buffer: [std.fs.MAX_PATH_BYTES]u8 = undefined;
    const path = try std.fs.realpath(rel_path, &path_buffer);

    const file = try std.fs.openFileAbsolute(path, .{ .read = true });
    defer file.close();

    const buffer_size = 2000;
    return try file.readToEndAlloc(allocator, buffer_size);
}
