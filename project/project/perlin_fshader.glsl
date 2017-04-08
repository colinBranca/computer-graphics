#version 330

in vec2 uv;
uniform int width;
uniform int height;
uniform int p[512];
uniform int octaves;

out vec3 color;

float fade(float t) { return t * t * t * (t * (t * 6 - 15) + 10); }

float lerp(float t, float a, float b) { return a + t * (b - a); }

float grad(int hash, float x, float y) {
      int h = hash & 15;                      // CONVERT LO 4 BITS OF HASH CODE
      float u = h<8 ? x : y,                 // INTO 12 GRADIENT DIRECTIONS.
             v = h<4 ? y : h==12||h==14 ? x : 0;
      return ((h&1) == 0 ? u : -u) + ((h&2) == 0 ? v : -v);
}

float noise(float x, float y) {
      int X = int(floor(x)) & 255;                  // FIND UNIT CUBE THAT
      int Y = int(floor(y)) & 255;                  // CONTAINS POINT.
      x -= floor(x);                                // FIND RELATIVE X,Y,Z
      y -= floor(y);                                // OF POINT IN CUBE.
      float u = fade(x);                                // COMPUTE FADE CURVES
      float v = fade(y);                                // FOR EACH OF X,Y,Z.
      int A = p[X  ]+Y, AA = p[A], AB = p[A+1],      // HASH COORDINATES OF
          B = p[X+1]+Y, BA = p[B], BB = p[B+1];      // THE 8 CUBE CORNERS,

      return lerp(0, lerp(v, lerp(u, grad(p[AA  ], x  , y  ),  // AND ADD
                                     grad(p[BA  ], x-1, y  )), // BLENDED
                             lerp(u, grad(p[AB  ], x  , y-1),  // RESULTS
                                     grad(p[BB  ], x-1, y-1))),// FROM  8
                     lerp(v, lerp(u, grad(p[AA+1], x  , y  ),  // CORNERS
                                     grad(p[BA+1], x-1, y  )), // OF CUBE
                             lerp(u, grad(p[AB+1], x  , y-1),
                                     grad(p[BB+1], x-1, y-1))));
   }

void main() {
	vec2 pos = uv;
        float frequency = 1.0f / 90.0f;
	float sum = 0.0f;
	float amplitude = 0.5f;
	for (int i = 0; i < octaves; ++i) {
		sum += amplitude * noise(pos.x * frequency, pos.y * frequency);
		pos *= 2.0f;
		amplitude *= 0.5;
	}
        color = vec3(sum, sum, sum);
}


