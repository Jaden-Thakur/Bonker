enum EntityType { PLAYER, PLATFORM, ENEMY };
enum EnemyType { JUMPY, SPIKY, DASHY };
enum AIMode { AGGRO, IDLE, PATROL };

#include "Map.h"

class Entity
{
private:
	

	int* m_left = NULL,
		* m_right = NULL;


	// PHYSICS (GRAVITY) 
	glm::vec3 m_position;
	glm::vec3 m_velocity;
	glm::vec3 m_acceleration;

	

	// TRANSFORMATIONS
	float m_speed;
	glm::vec3 m_movement;
	glm::mat4 m_model_matrix;



	float m_width = 0.8;
	float m_height = 1;

	EntityType m_entity_type;
	EnemyType m_enemy_type;
	AIMode m_ai_mode;

public:

	bool m_is_active = true;
	bool dead;
	bool m_is_jumping = false;
	float m_jumping_power = 5.0f;

	float timer = 0.0f;
	float dir = 1.0f;

	// ————— STATIC VARIABLES ————— //
	static const int SECONDS_PER_FRAME = 4;
	static const int LEFT = 1,
		RIGHT = 0;

	// ANIMATION
	int** m_animation = new int* [2]
		{
			m_left,
			m_right,	
		};

	int m_animation_frames = 0,
		m_animation_index = 0,
		m_animation_cols = 0,
		m_animation_rows = 0;

	int* m_animation_indices = NULL;
	float   m_animation_time = 0.0f;

	void activate_ai(Entity* player, float delta_time);
	void ai_attack(Entity* player);
	void ai_patrol(float delta_time);
	void ai_idle(float delta_time);

	bool m_can_damage = true;

	// PHYSICS COLLISIONS
	bool m_collided_top = false;
	bool m_collided_bottom = false;
	bool m_collided_left = false;
	bool m_collided_right = false;
	bool m_collided_entity_bottom = false;

	// TEXTURE
	GLuint m_texture_id;


	// METHODS
	Entity();
	~Entity();

	void scale();
	void update(float delta_time, Entity* player, Entity* collidatble_entities1, int collidable_entity_count, Map* map);
	void draw_sprite_from_texture_atlas(ShaderProgram* program, GLuint texture_id, int index);
	void render(ShaderProgram* program);

	bool const check_collision(Entity* collided_entity) const;
	void const check_collision_y(Entity* collidable_entities, int collidable_entity_count);
	void const check_collision_x(Entity*, int collidable_entity_count);
	void const check_collision_y(Map* map);
	void const check_collision_x(Map* map);

	void move_left() { m_movement.x = -m_speed; };
	void move_right() { m_movement.x = m_speed; };

	void rotate(float angle);

	void activate();
	void deactivate();

	// GETTERS
	EntityType const get_entity_type()    const { return m_entity_type; };
	EnemyType const get_enemy_type()    const { return m_enemy_type; };
	AIMode const get_mode()    const { return m_ai_mode; };
	glm::vec3  const get_position()       const { return m_position; };
	glm::vec3  const get_movement()       const { return m_movement; };
	glm::vec3  const get_velocity()       const { return m_velocity; };
	glm::vec3  const get_acceleration()       const { return m_acceleration; };
	float      const get_speed()          const { return m_speed; };
	float        const get_width()          const { return m_width; };
	float        const get_height()         const { return m_height; };

	// SETTERS
	void const set_entity_type(EntityType new_entity_type) { m_entity_type = new_entity_type; };
	void const set_enemy_type(EnemyType new_enemy_type) { m_enemy_type = new_enemy_type; };
	void const set_mode(AIMode new_mode) { m_ai_mode = new_mode; };
	void const set_position(glm::vec3 new_position) { m_position = new_position; };
	void const set_movement(glm::vec3 new_movement) { m_movement = new_movement; };
	void const set_velocity(glm::vec3 new_velocity) { m_velocity = new_velocity; };
	void const set_acceleration(glm::vec3 new_accel) { m_acceleration = new_accel; };
	void const set_speed(float new_speed) { m_speed = new_speed; };
	void const set_width(float new_width) { m_width = new_width; };
	void const set_height(float new_height) { m_height = new_height; };

};