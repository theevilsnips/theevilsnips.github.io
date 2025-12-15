// Written by Claude Haiku 4.5
// Simple terminal table-tennis game.
// Controls: 'w' = up, 's' = down, 'q' = quit
// Each round starts with N balls (N increases each round). The round
// ends when all balls leave the board (left or right).

#include <bits/stdc++.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>

using namespace std;

struct TermiosGuard {
    struct termios oldt;
    TermiosGuard() { tcgetattr(STDIN_FILENO, &oldt); struct termios newt = oldt; newt.c_lflag &= ~(ICANON | ECHO); tcsetattr(STDIN_FILENO, TCSANOW, &newt); }
    ~TermiosGuard() { tcsetattr(STDIN_FILENO, TCSANOW, &oldt); }
};

struct Ball {
    double x, y;
    double vx, vy;
    bool alive = true;
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    const int W = 60;
    const int H = 20;
    const int paddleX = W - 2;
    const int paddleHeight = 4;
    int paddleY = H/2 - paddleHeight/2;

    srand((unsigned)time(nullptr));

    TermiosGuard tg;
    // set non-blocking stdin
    int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);

    int roundNum = 1;
    bool running = true;

    auto clear_screen = [](){ cout << "\033[2J\033[H"; };
    auto hide_cursor = [](){ cout << "\033[?25l"; };
    auto show_cursor = [](){ cout << "\033[?25h"; };

    hide_cursor();

    while (running) {
        vector<Ball> balls;
        // spawn `round` balls in middle with random small velocities
        for (int i = 0; i < roundNum; ++i) {
            Ball b;
            b.x = W/4.0 + (rand()%5 - 2);
            b.y = H/2.0 + (rand()%5 - 2);
            double sx = (rand()%2) ? 1.0 : -1.0;
            double vy = ((rand()%200) - 100) / 100.0;
            b.vx = sx * (0.5 + (rand()%100)/100.0);
            b.vy = vy;
            b.alive = true;
            balls.push_back(b);
        }

        // round loop
        while (true) {
            // input
            char ch;
            ssize_t r = read(STDIN_FILENO, &ch, 1);
            if (r > 0) {
                if (ch == 'q') { running = false; break; }
                if (ch == 'w' || ch == 'W') paddleY = max(1, paddleY - 1);
                if (ch == 's' || ch == 'S') paddleY = min(H - paddleHeight - 1, paddleY + 1);
            }

            // update balls
            for (auto &b : balls) {
                if (!b.alive) continue;
                b.x += b.vx;
                b.y += b.vy;

                // reflect top/bottom
                if (b.y < 1) { b.y = 1; b.vy = -b.vy; }
                if (b.y > H-2) { b.y = H-2; b.vy = -b.vy; }

                // paddle collision (right side)
                if ((int)round(b.x) >= paddleX) {
                    if ((int)round(b.x) <= paddleX && (int)round(b.y) >= paddleY && (int)round(b.y) < paddleY + paddleHeight) {
                        b.x = paddleX - 1;
                        b.vx = -b.vx;
                        // tweak vy based on hit position
                        double hitPos = ((b.y - paddleY) / (double)paddleHeight) - 0.5;
                        b.vy += hitPos;
                    }
                }

                // left or right out of bounds -> remove
                if (b.x < 0 || b.x > W-1) {
                    b.alive = false;
                }
            }

            // check alive balls
            bool anyAlive = false;
            for (auto &b : balls) if (b.alive) { anyAlive = true; break; }
            if (!anyAlive) break;

            // render
            clear_screen();
            // top border
            for (int i = 0; i < W; ++i) cout << '-'; cout << '\n';
            for (int y = 1; y < H-1; ++y) {
                cout << '|';
                for (int x = 1; x < W-1; ++x) {
                    bool drawn = false;
                    // balls
                    for (auto &b : balls) {
                        if (!b.alive) continue;
                        int bx = (int)round(b.x);
                        int by = (int)round(b.y);
                        if (bx == x && by == y) { cout << 'o'; drawn = true; break; }
                    }
                    if (drawn) continue;

                    // paddle
                    if (x == paddleX) {
                        if (y >= paddleY && y < paddleY + paddleHeight) { cout << '|'; continue; }
                    }

                    cout << ' ';
                }
                cout << '|' << '\n';
            }
            // bottom border
            for (int i = 0; i < W; ++i) cout << '-'; cout << '\n';

            cout << "Round: " << roundNum << "  Balls alive: ";
            int aliveCount = 0; for (auto &b: balls) if (b.alive) ++aliveCount;
            cout << aliveCount << "  (w/s to move, q to quit)\n";

            cout.flush();

            usleep(50000); // 50ms frame
        }

        if (!running) break;

        // round complete
        clear_screen();
        cout << "Round " << roundNum << " complete! Press any key to continue (q to quit).\n";
        cout.flush();

        // wait for up to 5 seconds for key; if q then quit, otherwise continue
        bool cont = false;
        for (int i = 0; i < 100; ++i) {
            char ch;
            ssize_t r = read(STDIN_FILENO, &ch, 1);
            if (r > 0) {
                if (ch == 'q') { running = false; break; }
                cont = true; break;
            }
            usleep(50000);
        }
        roundNum++;
    }

    show_cursor();
    clear_screen();
    cout << "Goodbye!\n";
    return 0;
}
