const gl = @import("zgl");
const stb_image = @cImport({
    @cInclude("stb/stb_image.h");
});

pub fn loadTexture(path: [*]const u8, alpha: bool) gl.Texture {
    const texture = gl.genTexture();
    gl.Texture.bind(texture, .@"2d");

    gl.Texture.parameter(texture, .wrap_s, .repeat);
    gl.Texture.parameter(texture, .wrap_t, .repeat);
    gl.Texture.parameter(texture, .min_filter, .linear);
    gl.Texture.parameter(texture, .mag_filter, .linear);

    stb_image.stbi_set_flip_vertically_on_load(1);

    var width: c_int = 0;
    var height: c_int = 0;
    var channels: c_int = 0;

    const data: [*c]u8 = stb_image.stbi_load(path, &width, &height, &channels, 0);
    defer stb_image.stbi_image_free(data);

    if (data != null) {
        gl.textureImage2D(
            .@"2d",
            0,
            .rgb,
            @intCast(usize, width),
            @intCast(usize, height),
            if (alpha) .rgba else .rgb,
            .unsigned_byte,
            data,
        );
        gl.Texture.generateMipmap(texture);
    }

    return texture;
}
