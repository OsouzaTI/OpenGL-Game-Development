#pragma once

#define MAX_STRING_SIZE		500
#define MAX_TEXTURE_LAYERS	2

// Structs
typedef struct
{

	GLint	version;
	GLint	revision;

} MAP_VERSION;

typedef struct
{
	GLint		max_objects;
	GLint		max_lights;
	GLint		max_cameras;

	GLint		max_entities;
	GLint		max_items;
	GLint		max_sounds;

	GLboolean	use_skybox;
	GLboolean	use_fog;

} MAP_HEADER;

typedef struct
{

	char  filename[MAX_STRING_SIZE];
	GLint texid;

} MAP_SKYBOX_SIDE;

typedef struct
{

	MAP_SKYBOX_SIDE front;
	MAP_SKYBOX_SIDE back;
	MAP_SKYBOX_SIDE left;
	MAP_SKYBOX_SIDE right;
	MAP_SKYBOX_SIDE top;
	MAP_SKYBOX_SIDE bottom;

} MAP_SKYBOX;

typedef struct
{
	GLint	mode;
	GLfloat	start;
	GLfloat	end;
	GLfloat density;
	GLfloat	rgba[4];
} MAP_FOG;

typedef struct
{//TODO: estudar essa estrutura, apesar de pequena... � a mais complexa

	GLdouble	xyz[3];
	GLfloat		angle[3];

	GLubyte		select_rgb[3];

} MAP_STARTING_POSITION;

typedef struct
{

	char	map_name[MAX_STRING_SIZE];
	GLint	map_type;
	GLint	map_exit_rules;

	MAP_STARTING_POSITION single_player;
	MAP_STARTING_POSITION deathmatch[2];

} MAP_DETAILS;

typedef struct
{

	GLdouble	xyz[3];
	GLfloat		rgba[4];
	GLfloat		normal[3];
	GLfloat		fog_depth;

	GLubyte		select_rgb[3];

} MAP_VERTEX;

typedef struct
{

	GLfloat		uv1[2];
	GLfloat		uv2[2];
	GLfloat		uv3[2];

} MAP_UV_COORDS;

typedef struct
{
	
	GLint			point[3];
	MAP_UV_COORDS	uv[MAX_TEXTURE_LAYERS];

} MAP_TRIANGLE;

typedef struct
{

	char	filename[MAX_STRING_SIZE];
	GLint	id;
	GLint	style;
	GLint	blend_src;
	GLint	blend_dst;

} MAP_TEXTURE;

typedef struct 
{
	
	char			name[MAX_STRING_SIZE];
	GLint			type;
	GLint			special;

	GLboolean		is_collidable;
	GLboolean		is_visible;

	GLint			max_textures;
	GLint			max_triangles;
	GLint			max_vertices;

	MAP_TEXTURE		*texture;
	MAP_TRIANGLE	*triangle;
	MAP_VERTEX		*vertex;
	GLubyte			select_rgb[3];

} MAP_OBJECT;

typedef struct
{



} MAP_CAMERA;