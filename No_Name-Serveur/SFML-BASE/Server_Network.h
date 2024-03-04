#pragma once
#include "Clients.h"
#include "Projectile.h"
#include "Console.h"

class Room
{
public:
	enum class ROOM_STATE
	{
		RSNULL = -1,
		LOBBY,
		ROOM,
		GAME
	};
public:
	Room();
	Room(std::shared_ptr<Clients>& _room_clients, std::list<us>& _clients_ID, std::list<std::shared_ptr<Clients>>& _main_server_client, Console& _main_console, us _port);
	~Room();

#pragma region MAIN NETWORK FUNCTION	
	/// <summary>
	/// This update will manage the send and as an exemple the projectile.
	/// </summary>
	void update();
	
	/// <summary>
	/// <para>This update will manage the receive and verify if there is connection.</para>
	/// <para>And as it said in her name anything that will use a selector will be in it.</para>
	/// </summary>
	void update_selector();
	
	/// <summary>
	/// Updates the projectiles (The velocity and position).
	/// </summary>
	void update_projectiles();
#pragma endregion

#pragma region SUB NETWORK FUNCTION	
	/// <summary>
	/// Verifies the connection to the room.
	/// </summary>
	void verify_connection();
	
	/// <summary>
	/// Verify if a client is taking to much of his time to join the room.
	/// </summary>
	void client_timed_out();
	
	/// <summary>
	/// Receives anything that the clients send.
	/// </summary>
	void receive();
	
	/// <summary>
	/// Send anything that as been set to the clients.
	/// </summary>
	void send();

	/// <summary>
	/// <para>This function is made to leave the room if a client want to.</para>
	/// <para>The client will be moved to the lobby and the lobby will wait for the user to connect.</para>
	/// </summary>
	void client_leave_room();
	
	/// <summary>
	/// <para>Its just verify if the user is totally disconnected from the server.</para>
	/// <para>The only two way to get disconnected that the statue of the socket is Disconnected/Error</para>
	/// </summary>
	/// <param name="_packet">The packet to be tested when send.</param>
	void client_disconnection(sf::Packet& _packet);
#pragma endregion

#pragma region GETTER/SETTER
	us get_port() { return m_port; }
	us get_id() { return m_id; }
	float get_time_out_timer() { return m_time_out_timer; }
#pragma endregion

	void join_room(std::shared_ptr<Clients>& _client);
private:
	sf::SocketSelector m_selector;
	sf::TcpListener m_listener;

	sf::Time m_time;
	sf::Clock m_clock;

	inline void restart_clock() { m_time = m_clock.restart(); }
	inline float get_delta_time() { return m_time.asSeconds(); }
	
	us m_id;
	us m_port;

	std::string m_name;

	ROOM_STATE m_room_state;

	Console* m_main_server_console_ptr;

	std::list<us>* m_clients_ID;
	std::list<us> m_clients_ID_to_verify;
	std::list<std::shared_ptr<Clients>> m_clients; 
	std::list<std::shared_ptr<Clients>>* m_clients_main_server;
	std::list<std::unique_ptr<Projectile>> m_projectiles;

	std::thread m_update_thread;
	std::thread m_selector_thread;

	bool m_room_is_finish;
	bool m_client_want_to_leave_room;

	float m_sending_timer;
	float m_time_out_timer;

	unsigned m_projectiles_shooted;
};

class Server_Network
{
private:
	sf::TcpListener m_listener;
	sf::SocketSelector m_selector;

	std::thread m_server_thread;

	Console m_console;

	bool m_server_closed;
	bool m_client_want_to_create_room;

	float m_sending_timer;

	std::list<std::shared_ptr<Clients>> m_clients;
	std::list<std::unique_ptr<Room>> m_rooms;
	std::list<us> m_clients_ID_to_verify;
	std::list<us> m_clients_IDs;
public:
	Server_Network();
	~Server_Network();
	
#pragma region MAIN NETWORK FUNCTION	
	/// <summary>
	/// <para>This function is here to verify all the enter connection.</para>
	/// <para>And to verify if any packet need to be receive.</para>
	/// <para>This function is lauch in a thread.</para>
	/// <para>And to know why its lauch in a thread its because sf::Selector::wait() cause a lot of lag.</para>
	/// </summary>
	void server_thread();
	
	/// <summary>
	/// Just verify if there is connection or reconnection from a room.
	/// </summary>
	void verify_connection();

	/// <summary>
	/// Where all the information are receive by the main server.
	/// </summary>
	void receive();

	/// <summary>
	/// Where all the information are send to all the client in the main menu.
	/// </summary>
	void send();
#pragma endregion
	
#pragma region SUB NETWORK FUNCTION	
	/// <summary>
	/// This function return a unique ID that can be obtained more than one time.
	/// </summary>
	/// <returns></returns>
	us get_random_ID();

	/// <summary>
	/// <para>Verify if a room is timed out or not.</para>
	/// <para>(10 sec for a time out to occur.)</para>
	/// </summary>
	void room_client_verify();

	/// <summary>
	/// <para>This function is made to create a room if a client want to.</para>
	/// <para>The client will be moved to the room and the room will wait for the user to connect.</para>
	/// </summary>
	void client_create_room();
		
	/// <summary>
	/// <para>Its just verify if the user is totally disconnected from the server.</para>
	/// <para>The only two way to get disconnected that the statue of the socket is Disconnected/Error</para>
	/// </summary>
	/// <param name="_packet">The packet to be tested when send.</param>
	void client_disconnection(sf::Packet& _packet); //Need to add all the other packet (For later).
#pragma endregion

#pragma region GRAPHIC FUNCTION	
	/// <summary>
	/// Just an update.
	/// </summary>
	void update();
	
	/// <summary>
	/// Draws the specified window.
	/// </summary>
	/// <param name="_window">The window to be draw.</param>
	void draw(sf::RenderWindow& _window);
#pragma endregion
};

