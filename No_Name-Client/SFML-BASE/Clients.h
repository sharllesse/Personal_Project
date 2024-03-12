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
		LEAVE_ROOM,
		KICK_ROOM
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
		NAMESELECT,
		LOBBY,
		ROOM,
		GAME
	};
private:
	struct client_Information
	{
		client_Information() : 
			m_IP(""), m_disconnected(false), m_ID(0u)
		{}

		client_Information(std::string _IP, us _ID) :
			m_IP(_IP), m_disconnected(false), m_ID(_ID)
		{}

		client_Information(std::string _IP, us _ID, std::shared_ptr<CLIENT_STATE> _client_ptr) :
			m_IP(_IP), m_disconnected(false), m_ID(_ID), m_client_state(_client_ptr)
		{
			m_socket = std::make_unique<sf::TcpSocket>();
		}

		std::unique_ptr<sf::TcpSocket> m_socket;
		us m_ID;
		std::string m_IP;
		bool m_disconnected;

		std::shared_ptr<CLIENT_STATE> m_client_state;
	};

	struct client_plate
	{
		client_plate()
		{}

		client_plate(std::string _name, sf::Vector2f _position = sf::Vector2f())
		{
			m_kick_button = Button(sf::Vector2f(_position + sf::Vector2f(500.f, 32.5f)), sf::Vector2f(40.f, 40.f), GET_MANAGER->getTexture(""));

			m_crown_icon.setPosition(_position + sf::Vector2f(500.f, 32.5f));
			m_crown_icon.setSize(sf::Vector2f(60.f, 60.f));
			m_crown_icon.setTexture(&GET_MANAGER->getTexture(""));

			m_plate_background.setPosition(_position);
			m_plate_background.setSize(sf::Vector2f(450.f, 125.f));
			m_plate_background.setFillColor(sf::Color(150u, 150u, 150u, 255u));
			m_plate_background.setOutlineThickness(3.f);
			m_plate_background.setOutlineColor(sf::Color(100u, 100u, 100u, 255u));

			m_plate_name.setCharacterSize(50u);
			m_plate_name.setFont(GET_MANAGER->getFont("arial"));
			m_plate_name.setString(_name);
		}

		void set_position(sf::Vector2f _position)
		{
			m_kick_button.setPosition(_position + sf::Vector2f(500.f, 32.5f));
			m_crown_icon.setPosition(_position + sf::Vector2f(500.f, 32.5f));
			m_plate_background.setPosition(_position);
			m_plate_name.setPosition(
				m_plate_background.getPosition().x + (m_plate_background.getGlobalBounds().width / 2.f) - m_plate_name.getGlobalBounds().width / 2.f - m_plate_name.getLetterSpacing(),
				m_plate_background.getPosition().y + (m_plate_background.getGlobalBounds().height / 2.f) - m_plate_name.getGlobalBounds().height / 1.25f - m_plate_name.getLineSpacing()
			);
		}

		Button m_kick_button;

		sf::RectangleShape m_crown_icon;

		sf::RectangleShape m_plate_background;
		sf::Text m_plate_name;
		
	};

	sf::SocketSelector m_selector;

	std::thread m_receive_thread;
	std::mutex m_delete_client;
	std::mutex m_delete_projectiles;
	std::mutex m_delete_room;

	float m_sending_timer;

	bool m_game_is_finish;

	sf::Vector2f m_mouse_position;

	std::list<std::unique_ptr<Clients>> m_clients;
	std::list<std::unique_ptr<Projectile>> m_projectiles;

	//NAME ID PORT CLIENT_COUNT BUTTON
	std::vector<std::tuple<std::string, us, us, INT_TYPE, Button>> m_rooms;

	sf::RectangleShape m_all_clients;
	sf::CircleShape m_all_projectiles;

	sf::VertexArray m_aim_line;

	sf::View m_room_button_view;
	sf::RectangleShape test;

	float m_shoot_timer;

	bool m_shooted;
	bool m_waiting_for_reconnect;
	bool m_is_host;
	bool m_has_change_state;
	bool m_can_scroll_up;
	bool m_can_scroll_down;

	sf::Vector2f m_position;

	float m_speed;
	float m_rotation;

	std::string m_name;

	client_Information m_client_information;
	client_plate m_client_plate;
public:
	Clients();
	Clients(std::string _name, sf::Vector2f _position, float _speed, std::shared_ptr<Clients::CLIENT_STATE> _client_state_sptr);
	Clients(std::string _name, us _ID, std::string _IP);
	~Clients();

	bool connect_to_lobby(std::string _IP, us _port, float _time_out = 5.f);

	void disconnect_from_lobby();

	void create_room();
	void join_room(us _id, us _port);
	void leave_room();
	void kick_client(us _id);

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

	void update(WindowManager& _window);

	void draw(WindowManager& _window);

	void draw_rooms(WindowManager& _window);
	void draw_clients(WindowManager& _window);
	void draw_projectiles(WindowManager& _window);
	void draw_clients_plate(WindowManager& _window);

	void scroll_room(WindowManager& _window, float _speed);

	sf::Vector2f& get_position() { return m_position; }

	float& get_speed() { return m_speed; }
	float& get_rotation() { return m_rotation; }

	bool& is_host() { return m_is_host; }
	bool change_state(Clients::CLIENT_STATE _new_state) 
	{ 
		if (m_has_change_state)
		{
			(*m_client_information.m_client_state) = _new_state;
			m_has_change_state = false;

			return true;
		}

		return false;
	}

	std::string& get_name() { return m_name; }

	client_Information& get_client_information() { return m_client_information; }
};