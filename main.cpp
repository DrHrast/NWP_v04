#include "nwpwin.h"
#include <time.h>
#include <string.h>

// DID_IT: prepare class ("STATIC") for a ship
class Static : public vsite::nwp::window {
public:
    std::string class_name() override {
        return "STATIC";
    }
};

static const int ship_size = 50;
static const int obstacle_size = 50;
static const int projectile_size = 10;
static int projectile_direction;
static const int projectile_speed = 15;
static const int new_border_top = 50;
static int cur_score = 0;
static int h_score = 0;
//static int cur_time = 90;
//static constexpr UINT_PTR TIMER_ID = 1;

class main_window : public vsite::nwp::window
{
protected:

    RECT win_border;

    int main_window::on_create(CREATESTRUCT* pcs) override;

    void changeScore() {
        cur_score++;
        SetDlgItemText(*this, 3, std::to_string(cur_score).c_str());
    }

    void gameOver() {
        h_score = cur_score > h_score ? cur_score : h_score;
        cur_score = 0;


        SetDlgItemText(*this, 3, std::to_string(cur_score).c_str());
        SetDlgItemText(*this, 5, std::to_string(h_score).c_str());
    }

    bool checkCollision(const POINT& p1, const POINT& p2, int size_obj1, int size_obj2) {
        RECT rect_obj1 = { p1.x, p1.y, p1.x + size_obj1, p1.y + size_obj1 };
        RECT rect_obj2 = { p2.x, p2.y, p2.x + size_obj2, p2.y + size_obj2 };
        RECT rect_intersection;
        return IntersectRect(&rect_intersection, &rect_obj1, &rect_obj2);
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

    //void main_window::on_timer(HWND hw, UINT_PTR id) override {
    //    if (id == TIMER_ID) {
    //        // Decrease the time by 1 second
    //        cur_time--;

    //        // Update the label text with the new time
    //        std::string time_text = std::to_string(cur_time);
    //        SetDlgItemText(*this, 4, std::to_string(cur_time).c_str());

    //        // If time reaches zero, stop the timer
    //        if (cur_time <= 0) {
    //            gameOver();
    //        }
    //    }
    //}

    void moveProjectile() {
        switch (projectile_direction) {
        case 1:
            createProjectile(ship_position.x + ship_size / 2 - projectile_size / 2, ship_position.y - projectile_size);
            while (projectile_position.y > new_border_top) {
                if (checkCollision(projectile_position, obstacle_position, projectile_size, obstacle_size)) {
                    DestroyWindow(projectile);
                    moveObstacle();
                    changeScore();
                }
                projectile_position.y = max(projectile_position.y - projectile_speed, new_border_top);
                SetWindowPos(projectile, 0, projectile_position.x, projectile_position.y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
                InvalidateRect(*this, NULL, TRUE); //Repaint window
                UpdateWindow(*this); //Force update window
                Sleep(10);
            }
            DestroyWindow(projectile);
            break;
        case 2:
            createProjectile(ship_position.x + ship_size / 2 - projectile_size / 2, ship_position.y + ship_size);
            while (projectile_position.y < win_border.bottom - projectile_size) {
                if (checkCollision(projectile_position, obstacle_position, projectile_size, obstacle_size)) {
                    DestroyWindow(projectile);
                    moveObstacle();
                    changeScore();
                }
                projectile_position.y = min(projectile_position.y + projectile_speed, win_border.bottom - projectile_size);
                SetWindowPos(projectile, 0, projectile_position.x, projectile_position.y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
                InvalidateRect(*this, NULL, TRUE);
                UpdateWindow(*this);
                Sleep(10);
            }
            DestroyWindow(projectile);
            break;
        case 3:
            createProjectile(ship_position.x - projectile_size, ship_position.y + ship_size / 2 - projectile_size / 2);
            while (projectile_position.x > win_border.left) {
                if (checkCollision(projectile_position, obstacle_position, projectile_size, obstacle_size)) {
                    DestroyWindow(projectile);
                    moveObstacle();
                    changeScore();
                }
                projectile_position.x = max(projectile_position.x - projectile_speed, win_border.left);
                SetWindowPos(projectile, 0, projectile_position.x, projectile_position.y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
                InvalidateRect(*this, NULL, TRUE);
                UpdateWindow(*this);
                Sleep(10);
            }
            DestroyWindow(projectile);
            break;
        case 4:
            createProjectile(ship_position.x + ship_size, ship_position.y + ship_size / 2 - projectile_size / 2);
            while (projectile_position.x < win_border.right - projectile_size) {
                if (checkCollision(projectile_position, obstacle_position, projectile_size, obstacle_size)) {
                    DestroyWindow(projectile);
                    moveObstacle();
                    changeScore();
                }
                projectile_position.x = min(projectile_position.x + projectile_speed, win_border.right - projectile_size);
                SetWindowPos(projectile, 0, projectile_position.x, projectile_position.y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
                InvalidateRect(*this, NULL, TRUE);
                UpdateWindow(*this);
                Sleep(10);
            }
            DestroyWindow(projectile);
            break;
        }
    }

    void moveObstacle() {
        int max_x = win_border.right - obstacle_size;
        int max_y = win_border.bottom - obstacle_size;

        int new_x = rand() % max_x;
        int new_y = rand() % max_y;

        new_x = max(new_x, win_border.left);
        new_y = max(new_y, new_border_top);
        SetWindowPos(obstacle, 0, new_x, new_y, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
        obstacle_position.x = new_x;
        obstacle_position.y = new_y;
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

        if (ship) {
            switch (vk) {
            case VK_UP:
                projectile_direction = 1;
                ship_position.y = max(ship_position.y - moving_speed, new_border_top);
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

        if (checkCollision(ship_position, obstacle_position, ship_size, obstacle_size)) {
            //moveObstacle();
            ship_position.x = window_width / 2 - ship_size / 2;
            ship_position.y = window_height / 2 - ship_size / 2;
            gameOver();
            //SetWindowPos(ship, 0, ship_position.x, ship_position.y, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
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
public:
    Static score;
    //Static time;
    Static high_score;
    /*POINT score_pos;
    POINT time_pos;
    POINT high_score_pos;*/
};

int main_window::on_create(CREATESTRUCT* pcs) {
    score.create(*this, WS_CHILD | WS_VISIBLE | SS_CENTER | BS_CENTER | WS_BORDER, "0", 3, 30, 10, 50, 30);
    //time.create(*this, WS_CHILD | WS_VISIBLE | SS_CENTER | BS_CENTER | WS_BORDER, std::to_string(cur_time).c_str(), 4, 90, 10, 50, 30);
    high_score.create(*this, WS_CHILD | WS_VISIBLE | SS_CENTER | BS_CENTER | WS_BORDER, "0", 5, 150, 10, 50, 30);
    //SetTimer(*this, TIMER_ID, 1000, nullptr);

    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hp, LPSTR cmdLine, int nShow)
{
    srand(time(0));
    vsite::nwp::application app;
    main_window w;
    w.create(0, WS_OVERLAPPEDWINDOW | WS_VISIBLE, "NWP 4");
    return app.run();
}