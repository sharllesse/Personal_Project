#pragma once
#include "Projectile.h"
#include "Button.h"

class Clients
{
public:
	enum INFO_TYPE_CLIENT_SIDE
	{
		ITCNULL = -1,
		TRANSFORM,
		SHOOT,
		CREATE_ROOM,
		JOIN_ROOM,
		LEAVE_ROOM
	};

	enum INFO_TYPE_SERVER_SIDE
	{
		ITSNULL = -1,
		CLIENT_INFORMATION,
		JOIN_INFORMATION,
		DISCONNECTED_INFORMATION,
		PROJECTILES_INFORMATION,
		LOBBY_TO_ROOM_INFORMATION,
		ROOM_TIMED_OUT,
		ROOM_TO_LOBBY_INFORMATION,
		ROOM_INFORMATION
	};

	enum class CLIENT_STATE
	{
		CSNULL = -1,
		LOBBY,
		ROOM,
		GAME
	};
private:
	struct client_Information
	{
		client_Information() : 
			m_IP(""), m_disconnected(false), m_ID(0u), m_client_state(CLIENT_STATE::LOBBY)
		{}

		client_Information(std::string _IP, us _ID) :
			m_IP(""), m_disconnected(false), m_ID(0u), m_client_state(CLIENT_STATE::LOBBY)
		{}

		client_Information(std::string _IP, us _ID, bool _is_main_client) :
			m_IP(_IP), m_disconnected(false), m_ID(_ID), m_client_state(CLIENT_STATE::LOBBY)
		{
			m_socket = std::make_unique<sf::TcpSocket>();
		}

		std::unique_ptr<sf::TcpSocket> m_socket;
		us m_ID;
		std::string m_IP;
		bool m_disconnected;

		CLIENT_STATE m_client_state;
	};

	sf::SocketSelector m_selector;

	std::thread m_receive_thread;
	std::mutex m_delete_client;
	std::mutex m_delete_projectiles;

	float m_sending_timer;

	bool m_game_is_finish;

	sf::Vector2f m_mouse_position;

	std::list<std::shared_ptr<Clients>> m_clients;
	std::list<std::unique_ptr<Projectile>> m_projectiles;

	std::vector<std::tuple<std::string, us, us, Button>> m_rooms;

	sf::RectangleShape m_all_clients;
	sf::CircleShape m_all_projectiles;

	sf::VertexArray m_aim_line;

	float m_shoot_timer;

	bool m_shooted;
	bool m_waiting_for_reconnect;

	sf::Vector2f m_position;

	float m_speed;
	float m_rotation;

	std::string m_name;

	client_Information m_client_information;
public:
	Clients();
	Clients(std::string _name, sf::Vector2f _position, float _speed);
	Clients(std::string _name, us _ID, std::string _IP);
	~Clients();

	bool connect_to_lobby(std::string _IP, us _port, float _time_out = 5.f);

	void disconnect_from_lobby();

	void create_room();
	void leave_room();

	void room_connection(sf::Packet& _packet);
	void server_connection(sf::Packet& _packet);
	void clients_information(sf::Packet& _packet);
	void clients_connected(sf::Packet& _packet);
	void clients_disconnected(sf::Packet& _packet);
	void projectiles_information(sf::Packet& _packet);
	void room_information(sf::Packet& _packet);

	void receive();
	void send();

	//Faire une fonction permetant de créer un lobby et dans d'un join un
	//Et aussi faire une enum pour savoir ou est le joueur LOBBY/ROOM/GAME etc.

	sf::Socket::Status send_packet(sf::Packet& _packet);
	sf::Socket::Status receive_packet(sf::Packet& _packet);

	void update(sf::RenderWindow& _window);

	void draw(sf::RenderWindow& _window);

	void draw_clients(sf::RenderWindow& _window);
	void draw_projectiles(sf::RenderWindow& _window);

	sf::Vector2f& get_position() { return m_position; }
	float& get_speed() { return m_speed; }
	float& get_rotation() { return m_rotation; }
	std::string& get_name() { return m_name; }
	client_Information& get_client_information() { return m_client_information; }
};