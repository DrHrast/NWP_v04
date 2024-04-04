#include "nwpwin.h"
#include <time.h>

// DID_IT: prepare class ("STATIC") for a ship
class Static : public vsite::nwp::window {
public:
	std::string class_name() override {
		return "STATIC";
	}
};

static const int ship_size = 50;
static const int obstacle_size = 50;
static const int projectile_size = 30;
static int projectile_direction;
static const int projectile_speed = 50;

class main_window : public vsite::nwp::window
{
protected:

	RECT win_border;

	bool checkCollision(const POINT& p1, const POINT& p2) {
		RECT rect_ship = { p1.x, p1.y, p1.x + ship_size, p1.y + ship_size };
		RECT rect_obstacle = { p2.x, p2.y, p2.x + obstacle_size, p2.y + obstacle_size };
		RECT rect_intersection;
		return IntersectRect(&rect_intersection, &rect_ship, &rect_obstacle);
	}

	void createObstacle(int x, int y) {
		obstacle.create(*this, WS_CHILD | WS_VISIBLE | SS_CENTER | WS_DLGFRAME | SWP_NOZORDER, "\no", 1, x, y, obstacle_size, obstacle_size);
		SetWindowLongPtr(obstacle, GWL_EXSTYLE, GetWindowLongPtr(obstacle, GWL_EXSTYLE) | WS_EX_STATICEDGE);

		obstacle_position.x = x;
		obstacle_position.y = y;
	}

	void createProjectile(int x, int y) {
		projectile.create(*this, WS_CHILD | WS_VISIBLE | SS_CENTER | WS_DLGFRAME | SWP_NOZORDER, "o", 2, x, y, projectile_size, projectile_size);
		SetWindowLongPtr(projectile, GWL_EXSTYLE, GetWindowLongPtr(projectile, GWL_EXSTYLE) | WS_EX_STATICEDGE);

		projectile_position.x = x;
		projectile_position.y = y;
	}

	void moveProjectile() {
		switch (projectile_direction) {
		case 1:
			createProjectile(ship_position.x + ship_size / 3, ship_position.y - ship_size / 4);
			break;
		case 2:
			break;
		case 3:
			break;
		case 4:
			break;
		}
	}

	void on_left_button_down(POINT p) override { 
		// DID_IT: create ship if it doesn't exist yet
		ship_position = p;
		GetClientRect(*this, &win_border);

		int window_width = win_border.right - win_border.left;
		int window_height = win_border.bottom - win_border.top;

		int x = max(0, min(rand() % window_width, window_width - obstacle_size));
		int y = max(0, min(rand() % window_height, window_height - obstacle_size));

		if (!ship) {
			ship.create(*this, WS_CHILD | WS_VISIBLE | SS_CENTER, "\nx", 0, p.x, p.y, ship_size, ship_size);
			createObstacle(x, y);
		}
		// DID_IT: change current location
		else {
			SetWindowPos(ship, 0, p.x, p.y, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
		}
	}
	void on_key_up(int vk) override {
		// DID_IT: mark ship (if exists) as "not moving"
		DWORD style = GetWindowLong(ship, GWL_STYLE);
		style &= ~WS_BORDER;
		SetWindowLong(ship, GWL_STYLE, style);
		SetWindowPos(ship, 0, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_FRAMECHANGED | SWP_NOMOVE);
	}
	void on_key_down(int vk) override {
		// DID_IT: if ship exists, move it depending on key and mark as "moving"
		int moving_speed = GetAsyncKeyState(VK_CONTROL) ? 40 : 20;
		GetClientRect(*this, &win_border);
		int window_width = win_border.right - win_border.left;
		int window_height = win_border.bottom - win_border.top;
		int projectile_direction;

		if (ship) {
			switch (vk) {
			case VK_UP:
				projectile_direction = 1;
				ship_position.y = max(ship_position.y - moving_speed, win_border.top);
				SetWindowText(ship, "/\\\nx");
				break;
			case VK_DOWN:
				projectile_direction = 2;
				ship_position.y = min(ship_position.y + moving_speed, win_border.bottom - ship_size);
				SetWindowText(ship, "\nx\nV");
				break;
			case VK_LEFT:
				projectile_direction = 3;
				ship_position.x = max(ship_position.x - moving_speed, win_border.left);
				SetWindowText(ship, "\n<  x   ");
				break;
			case VK_RIGHT:
				projectile_direction = 4;
				ship_position.x = min(ship_position.x + moving_speed, win_border.right - ship_size);
				SetWindowText(ship, "\n   x  >");
				break;
			case VK_SPACE:
				//projectile
				moveProjectile();
				break;
			}
		}

		if (checkCollision(ship_position, obstacle_position)) {

			int new_x = max(0, min(rand() % window_width, window_width - obstacle_size));
			int new_y = max(0, min(rand() % window_height, window_height - obstacle_size));
			SetWindowPos(obstacle, 0, new_x, new_y, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
			obstacle_position.x = new_x;
			obstacle_position.y = new_y;
		}

		DWORD style = GetWindowLong(ship, GWL_STYLE);
		SetWindowLong(ship, GWL_STYLE, style | WS_BORDER);
		SetWindowPos(ship, 0, ship_position.x, ship_position.y, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
	}
	void on_destroy() override {
		::PostQuitMessage(0);
	}
private:
	Static ship;
	Static obstacle;
	Static projectile;
	POINT ship_position;
	POINT obstacle_position;
	POINT projectile_position;
};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hp, LPSTR cmdLine, int nShow)
{
	srand(time(0));
	vsite::nwp::application app;
	main_window w;
	w.create(0, WS_OVERLAPPEDWINDOW | WS_VISIBLE, "NWP 4");
	return app.run();
}
