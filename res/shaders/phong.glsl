#shader vertex
#version 330 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 norm;

out vec3 normal;
out vec3 fragPosition;

uniform mat4 m = mat4(1.0);
uniform mat4 mvp = mat4(1.0);

void main()
{
    gl_Position = mvp * vec4(pos, 1);

    normal = normalize(mat3(transpose(inverse(m))) * norm);

    fragPosition = vec3(m * vec4(pos, 1));
}

#shader fragment
#version 330 core

in vec3 normal;
in vec3 fragPosition;

out vec3 color;

uniform vec3 viewPos = vec3(0, 0, 0);

uniform vec3 objectColor = vec3(1, 0, 0);

uniform vec3 lightPos = vec3(0, 1, 0);
uniform vec3 lightColor = vec3(1, 1, 1);

float ambientPower = 0.5;
float diffusePower = 0.5;
float specularPower = 0.0;

void main()
{
    vec3 ambient = ambientPower * lightColor;

    vec3 lightDir = normalize(lightPos - fragPosition);
    float NdotL = max(dot(normal, lightDir), 0);

    vec3 diffuse =  diffusePower * NdotL * lightColor;

    vec3 viewDir = normalize(viewPos - fragPosition);
    vec3 reflectDir = reflect(-lightDir, normal);
    float VdotR = max(dot(viewDir, reflectDir), 0.0);

    vec3 specular = specularPower * pow(VdotR, 16) * lightColor;

    color = (ambient + diffuse + specular) * objectColor;
}