#version 330
flat in float colID;

layout(location = 0)out vec3 colori;

void main()
{
    colori = vec3(0.0, colID, colID);
}