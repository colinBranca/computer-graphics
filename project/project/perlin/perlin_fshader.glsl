#version 330

in vec2 uv;

uniform int p[512];
uniform int octaves;
uniform float frequencyX;
uniform float frequencyY;
uniform float amplitude;
uniform float seedX;
uniform float seedY;

out float color;

float fade(float t) { return t * t * t * (t * (t * 6 - 15) + 10); }

float lerp(float t, float a, float b) { return a + t * (b - a); }

float grad(int hash, float x, float y) {
      int h = hash & 15;
      float u = h<8 ? x : y,
             v = h<4 ? y : h==12||h==14 ? x : 0;
      return ((h&1) == 0 ? u : -u) + ((h&2) == 0 ? v : -v);
}

float noise(float x, float y) {
      int X = int(floor(x)) & 255;
      int Y = int(floor(y)) & 255;
      x -= floor(x);
      y -= floor(y);
      float u = fade(x);
      float v = fade(y);
      int A = p[X] + Y;
      int B = p[X + 1] + Y;

      return lerp(v,
                  lerp(u, grad(p[A], x, y), grad(p[B], x-1, y)),
                  lerp(u, grad(p[A+1], x, y -1), grad(p[B+1], x-1, y-1)));
   }

void main() {
	vec2 pos = uv;
	float sum = 0.0f;
	float a = amplitude;

	for (int i = 0; i < octaves; ++i) {
		sum += a * noise(seedX * pos.x * frequencyX, seedY * pos.y * frequencyY);
		pos *= 2.0f;
		a *= 0.5f;
	}

        color = sum + 0.5f;
}
