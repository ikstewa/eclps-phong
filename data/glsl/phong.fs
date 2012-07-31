// Phong shader
#define MAX_LIGHTS gl_MaxLights
//#define MAX_LIGHTS 2

uniform sampler2D imgTexture;

varying vec3 normal;
varying vec4 ecPosition;

//uniform float cos_outer_cone_angle[MAX_LIGHTS];
uniform float cos_outer_cone_angle;

void mrt(vec4);



vec4 phong(in vec3 N, in vec3 eyeVec);

void main()
{
   // normalize normal
   vec3 N = normalize(normal);
   // vertex to camera in eye coords
   vec3 eyeVec = vec3(normalize(-ecPosition));

   
   // phong shading
   //mrt(phong(N, eyeVec) * texture2D(imgTexture, gl_TexCoord[0].xy));
   mrt(phong(N, eyeVec));

   // texture
   //mrt(texture2D(imgTexture, gl_TexCoord[0].xy));

   


}

vec4 phong(in vec3 N, in vec3 eyeVec)
{

   // start with the ambient colors
   vec4 final_color = (gl_FrontLightModelProduct.sceneColor * gl_FrontMaterial.ambient);

   // perform lighting per light
   for (int i = 0; i < MAX_LIGHTS; i++)
   {
      // add the ambient for this light
      final_color += (gl_LightSource[i].ambient * gl_FrontMaterial.ambient);

      // Directional vector: eyespace position to light
      vec3 aux = (vec3(gl_LightSource[i].position - ecPosition));
      vec3 lightDir = normalize(aux);
      float d = length(aux);
      
      float att = 1.0 / ( gl_LightSource[i].constantAttenuation +
      					  gl_LightSource[i].linearAttenuation*d +
      					  gl_LightSource[i].quadraticAttenuation*d*d);
      
      
      
      // spot light direction
      vec3 spotDir = normalize(gl_LightSource[i].spotDirection);
      
      float cos_cur_angle = dot(-lightDir, spotDir);
      float cos_inner_cone_angle = gl_LightSource[i].spotCosCutoff;
      float cos_inner_minus_outer_angle = cos_inner_cone_angle - cos_outer_cone_angle;
      
      // spot light
      float spot = 0.0;
      /*
      spot = clamp((cos_cur_angle - cos_outer_cone_angle) /
      				cos_inner_minus_outer_angle, 0.0, 1.0);
                  */
      spot = 1.0;
      

      // NdotL: normal . light dir
      float NdotL = max( dot( N, lightDir ), 0.0 );
	
      // if pixel is illuminated
      if (NdotL > 0.0) 
      {
         // diffuse component
         final_color += gl_LightSource[i].diffuse *
                        gl_FrontMaterial.diffuse *
                        NdotL * (spot) * att;
                        
         // specular component
         vec3 E = eyeVec;
         vec3 R = reflect(-lightDir, N);
         float specular = pow( max(dot(R, E), 0.0), gl_FrontMaterial.shininess );
         final_color += gl_LightSource[i].specular * gl_FrontMaterial.specular * specular * spot * att;
      }
   }

   return final_color;

}

