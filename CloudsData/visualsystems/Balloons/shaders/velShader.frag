#version 110
#extension GL_ARB_texture_rectangle : enable

uniform sampler2DRect posTexture;
uniform sampler2DRect velTexture;



uniform float highSpeedScale;
uniform float speedLow;
uniform float speedHi;
uniform float highSpeedPercent;


uniform vec3 camPos;

uniform float bound;
uniform float dimX;
uniform float dimY;

uniform float noiseScl;
uniform float time;
uniform float offset;
uniform float noiseSampleScale;
uniform float velAtten;
uniform float radius;
uniform float accScl;
uniform float gravity;
uniform float attractionToCenter;
uniform float cameraBounce;

uniform vec3 camOffset;

uniform float netHeight;

varying vec2 uv;

float hash( float n ) { return fract(sin(n)*43758.5453123); }
float noise( in vec3 x )
{
    vec3 p = floor(x);
    vec3 f = fract(x);
    f = f*f*(3.0-2.0*f);
	
    float n = p.x + p.y*157.0 + 113.0*p.z;
    return mix(mix(mix( hash(n+  0.0), hash(n+  1.0),f.x),
                   mix( hash(n+157.0), hash(n+158.0),f.x),f.y),
               mix(mix( hash(n+113.0), hash(n+114.0),f.x),
                   mix( hash(n+270.0), hash(n+271.0),f.x),f.y),f.z);
}

float _noise(vec3 p) //Thx to Las^Mercury
{
	vec3 i = floor(p);
	vec4 a = dot(i, vec3(1., 57., 21.)) + vec4(0., 57., 21., 78.);
	vec3 f = cos((p-i)*acos(-1.))*(-.5)+.5;
	a = mix(sin(cos(a)*a),sin(cos(1.+a)*(1.+a)), f.x);
	a.xy = mix(a.xz, a.yw, f.y);
	return mix(a.x, a.y, f.z);
}

float noise( float x, float y, float z)
{
	return _noise( vec3(x,y,z) );
}

void bounce(inout vec3 acc, in vec3 pos, in vec3 p, inout int collisionCount, float radius)
{
	vec3 delta = p - pos;//texture2DRect( posTexture, vec2(float(i), float(j))).xyz - pos;
	vec3 deltaMag = abs(delta);
	if( deltaMag.y < radius && deltaMag.x < radius && deltaMag.z < radius)
	{
		float dist = deltaMag.x + deltaMag.y + deltaMag.z;
		if(dist < radius)
		{
			acc -= normalize(delta) * (1. - dist / radius);
			collisionCount++;
		}
	}
}

void main()
{
//	//TODO: make these uniforms
//	float noiseScl = .4, offset = .1, noiseSampleScale = .01, velAtten = .97, radius = 25., accScl = .2, gravity = .02, attractionToCenter = .01, cameraBounce = 10.;
	
	vec3 pos = texture2DRect( posTexture, uv).xyz;
	
//	pos -= camOffset*.5;
	
	vec3 vel = texture2DRect( velTexture, uv).xyz;
	
	vec3 p = pos * noiseSampleScale;
//	p.y += time + camOffset.y;
	
	p.y += time;
	p.y -= camOffset.y;
	vec3 acc = vec3(0.,0.,0.);
	
	//noise influence
	acc.x = noise(p.x+offset, p.y, p.z) - noise(p.x-offset, p.y, p.z);
	acc.y = noise(p.x, p.y+offset, p.z) - noise(p.x, p.y-offset, p.z);
	acc.z = noise(p.x, p.y, p.z+offset) - noise(p.x, p.y, p.z-offset);
	acc *= noiseScl;
	
	//collision testing
	vec3 delta, deltaMag;
	float dist;
	int collisionCount = 0;
	for (int i=0; i<int(dimX); i++)
	{
		for (int j=0; j<int(dimY); j++)
		{
			if(collisionCount<6 && j != i)
			{
				bounce(acc, pos, texture2DRect( posTexture, vec2(float(i), float(j))).xyz, collisionCount, radius);
			}
		}
	}
	
	//repel from camera
	bounce(acc, pos, camPos, collisionCount, radius * 2.5);

	//attract them to the center axis
	acc.xz -= normalize(pos.xz) * attractionToCenter;
	
	acc *= accScl;
	acc.y += gravity;
	
	//gravity with varience based on index and other stuff...
	float i = uv.x * dimY + uv.y;
	float total = dimX * dimY;
	if( i < total*highSpeedPercent)
	{
		acc.y *= highSpeedScale * mix(speedLow, speedHi, i / total*highSpeedPercent);
	}
	else
	{
		acc.y *= mix(speedLow, speedHi, (i-total) / (total * (1.- highSpeedPercent)));
	}
	
	
	//acceleration & gravity
	vel *= velAtten;
	vel += acc;
	
	//draw it
   	gl_FragColor = vec4(vel, 1.0);
}