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

float grad(int hash, float x, float y, float z) {
      int h = hash & 15;
      float u = h<8 ? x : y,
            v = h<4 ? y : h==12||h==14 ? x : z;
      return ((h&1) == 0 ? u : -u) + ((h&2) == 0 ? v : -v);
}

float noise(float x, float y, float z) {
      int X = int(floor(x)) & 255;
      int Y = int(floor(y)) & 255;
      int Z = int(floor(z)) & 255;
      x -= floor(x);
      y -= floor(y);
      z -= floor(z);
      float u = fade(x);
      float v = fade(y);
      float w = fade(z);
      int A = p[X] + Y;
      int AA = p[A] + Z;
      int AB = p[A+1] + Z;
      int B = p[X + 1] + Y;
      int BA = p[B] + Z;
      int BB = p[B+1] + Z;

      return lerp(w, lerp(v, lerp(u, grad(p[AA  ], x  , y  , z   ),
                                     grad(p[BA  ], x-1, y  , z   )),
                             lerp(u, grad(p[AB  ], x  , y-1, z   ),
                                     grad(p[BB  ], x-1, y-1, z   ))),
                     lerp(v, lerp(u, grad(p[AA+1], x  , y  , z-1 ),
                                     grad(p[BA+1], x-1, y  , z-1 )),
                             lerp(u, grad(p[AB+1], x  , y-1, z-1 ),
                                     grad(p[BB+1], x-1, y-1, z-1 ))));
   }

void main() {
	vec2 pos = uv;
	float sum = 0.0f;
	float a = amplitude;
      float freqX = frequencyX;
      float freqY = frequencyY;
      float fact = 2.0f;

	for (int i = 0; i < octaves; ++i) {
		sum += a * noise(pos.x * freqX, pos.y * freqY, 0);
            freqX *= fact;
            freqY *= fact;
		a *= 0.5f;
	}

        color = sum; //+ 0.5f;
}
