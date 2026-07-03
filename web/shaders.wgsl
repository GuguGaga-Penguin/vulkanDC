struct VertexInput {
    @location(0) position: vec3<f32>,
    @location(1) uv: vec2<f32>,
};

struct VertexOutput {
    @builtin(position) position: vec4<f32>,
    @location(0) uv: vec2<f32>,
};

@vertex
fn vs_main(model: VertexInput) -> VertexOutput {
    var out: VertexOutput;
    out.position = vec4<f32>(model.position, 1.0);
    out.uv = model.uv;
    return out;
}

@group(0) @binding(0) var qSampler: sampler;
@group(0) @binding(1) var qTexture: texture_2d<f32>;

@fragment
fn fs_main(in: VertexOutput) -> @location(0) vec4<f32> {
    return textureSample(qTexture, qSampler, in.uv);
}
