//Based on "Real-Time Fluid Dynamics for Games" by Jos Stam
const canvas = document.getElementById("canvas");
const ctx = canvas.getContext("2d");
const N = 64;
const size = (N + 2) * (N + 2);
let u = new Array(size).fill(0);
let v = new Array(size).fill(0);
let u_prev = new Array(size).fill(0);
let v_prev = new Array(size).fill(0);
let dens = new Array(size).fill(0);
let dens_prev = new Array(size).fill(0);
let source = new Array(size).fill(0);
const IX = (i, j) => i + (N + 2) * j;
function add_source(x, s, dt) {
  for (let i = 0; i < size; i++) {
    x[i] += dt * s[i];
  }
}
function diffuse(b, x, x0, diff, dt) {
    const a=dt * diff * N * N;
    for (let k = 0; k < 20; k++) {
        for (let j = 1; j <= N; j++) {
            for (let i = 1; i <= N; i++) {
                x[IX(i, j)] = (x0[IX(i, j)] + a * (x[IX(i - 1, j)] + x[IX(i + 1, j)] + x[IX(i, j - 1)] + x[IX(i, j + 1)])) / (1 + 4 * a);
            }
        }
        boundary(b, x);
    }
}
function advect(b, d, d0, u, v, dt) {
    const dt0 = dt * N;
    for (let j = 1; j <= N; j++) {
        for (let i = 1; i <= N; i++) {
            let x = i - dt0 * u[IX(i, j)];
            let y = j - dt0 * v[IX(i, j)];
            if (x < 0.5) x = 0.5;
            if (x > N + 0.5) x = N + 0.5;
            const i0 = Math.floor(x);
            const i1 = i0 + 1;
            if (y < 0.5) y = 0.5;
            if (y > N + 0.5) y = N + 0.5;
            const j0 = Math.floor(y);
            const j1 = j0 + 1;
            const s1 = x - i0;
            const s0 = 1 - s1;
            const t1 = y - j0;
            const t0 = 1 - t1;
            d[IX(i, j)] =
                s0 * (t0 * d0[IX(i0, j0)] + t1 * d0[IX(i0, j1)]) +
                s1 * (t0 * d0[IX(i1, j0)] + t1 * d0[IX(i1, j1)]);
        }
    }
    boundary(b, d);
}
function boundary(b, x) {
    for (let i = 1; i <= N; i++) {
        x[IX(0, i)] = b === 1 ? -x[IX(1, i)] : x[IX(1, i)];
        x[IX(N + 1, i)] = b === 1 ? -x[IX(N, i)] : x[IX(N, i)];
        x[IX(i, 0)] = b === 2 ? -x[IX(i, 1)] : x[IX(i, 1)];
        x[IX(i, N + 1)] = b === 2 ? -x[IX(i, N)] : x[IX(i, N)];
    }
    x[IX(0, 0)] = 0.5 * (x[IX(1, 0)] + x[IX(0, 1)]);
    x[IX(0, N + 1)] = 0.5 * (x[IX(1, N + 1)] + x[IX(0, N)]);
    x[IX(N + 1, 0)] = 0.5 * (x[IX(N, 0)] + x[IX(N + 1, 1)]);
    x[IX(N + 1, N + 1)] = 0.5 * (x[IX(N, N + 1)] + x[IX(N + 1, N)]);
}
function project(u, v, p, div) {
    for (let j = 1; j <= N; j++) {
        for (let i = 1; i <= N; i++) {
            div[IX(i, j)] = -0.5 * (u[IX(i + 1, j)] - u[IX(i - 1, j)] + v[IX(i, j + 1)] - v[IX(i, j - 1)]) / N;
            p[IX(i, j)] = 0;
        }
    }
    boundary(0, div);
    boundary(0, p);
    for (let k = 0; k < 20; k++) {
        for (let j = 1; j <= N; j++) {
            for (let i = 1; i <= N; i++) {
                p[IX(i, j)] = (div[IX(i, j)] + p[IX(i - 1, j)] + p[IX(i + 1, j)] + p[IX(i, j - 1)] + p[IX(i, j + 1)]) / 4;
            }
        }
        boundary(0, p);
    }
    for (let j = 1; j <= N; j++) {
        for (let i = 1; i <= N; i++) {
            u[IX(i, j)] -= 0.5 * N * (p[IX(i + 1, j)] - p[IX(i - 1, j)]);
            v[IX(i, j)] -= 0.5 * N * (p[IX(i, j + 1)] - p[IX(i, j - 1)]);
        }
    }
    boundary(1, u);
    boundary(2, v);
}
function step(dt) {
    add_source(u, u_prev, dt);
    add_source(v, v_prev, dt);
    add_source(dens, dens_prev, dt);

    [u_prev, u] = [u, u_prev];
    diffuse(1, u, u_prev, 0.0001, dt);

    [v_prev, v] = [v, v_prev];
    diffuse(2, v, v_prev, 0.0001, dt);

    project(u, v, u_prev, v_prev);

    [u_prev, u] = [u, u_prev];
    [v_prev, v] = [v, v_prev];
    advect(1, u, u_prev, u_prev, v_prev, dt);
    advect(2, v, v_prev, u_prev, v_prev, dt);

    project(u, v, u_prev, v_prev);

    [dens_prev, dens] = [dens, dens_prev];
    diffuse(0, dens, dens_prev, 0.0001, dt);
    [dens_prev, dens] = [dens, dens_prev];
    advect(0, dens, dens_prev, u, v, dt);

    u_prev.fill(0);
    v_prev.fill(0);
    dens_prev.fill(0);
}
function draw() {
    ctx.clearRect(0, 0, canvas.width, canvas.height);
    const cellWidth = canvas.width / N;
    const cellHeight = canvas.height / N;
    for (let j = 1; j <= N; j++) {
        for (let i = 1; i <= N; i++) {
            const density = dens[IX(i, j)];
            const colorValue = Math.min(255, Math.max(0, Math.floor(density * 255)));
            ctx.fillStyle = `rgb(${colorValue}, ${colorValue}, ${colorValue})`;
            ctx.fillRect((i - 1) * cellWidth, (j - 1) * cellHeight, cellWidth, cellHeight);
        }
    }
}
function random_fill(u, bias, height) {
    for (let j = 1; j <= N; j++) {
        for (let i = 1; i <= N; i++) {
            u[IX(i, j)] = Math.random() * height + bias;
        }
    }
}
function init() {
    random_fill(u, 0, 0.1);
    random_fill(v, 0, 0.1);
    u_prev.fill(0);
    v_prev.fill(0);
    random_fill(dens, 0, 1); // Fill density with random values between 0 and 1
    dens_prev.fill(0);
}
let lastTime = performance.now();
function animate() {
    const currentTime = performance.now();
    const dt = (currentTime - lastTime) / 1000; // Convert to seconds
    lastTime = currentTime;

    step(dt);
    draw();

    requestAnimationFrame(animate);
}
init();
animate();
const reset_button = document.getElementById('reset');
reset_button.addEventListener('click', () => {
    init();
});
const pause_button = document.getElementById('pause');
let paused = false;
pause_button.addEventListener('click', () => {
    paused = !paused;
    if (!paused) {
        lastTime = performance.now(); // Reset lastTime to avoid large dt after pause
        animate();
    } else {
        cancelAnimationFrame(animate);
    }
});
let do_paint=false;
const toggle_paint=document.getElementById("toggle_paint")
class UserInteraction {
  constructor(canvas) {
    this.canvas = canvas;
    this.isDragging = false;
    this.lastX = 0;
    this.lastY = 0;

    this.canvas.addEventListener('mousedown', (e) => {
      this.isDragging = true;
      const rect = this.canvas.getBoundingClientRect();
      this.lastX = e.clientX - rect.left;
      this.lastY = e.clientY - rect.top;
    });

    this.canvas.addEventListener('mousemove', (e) => {
      if (this.isDragging) {
        const rect = this.canvas.getBoundingClientRect();
        const currentX = e.clientX - rect.left;
        const currentY = e.clientY - rect.top;

        // Calculate the change in position
        const dx = currentX - this.lastX;
        const dy = currentY - this.lastY;

        // Update the fluid velocity based on the drag direction and magnitude
        const i = Math.floor((currentX / this.canvas.width) * N) + 1;
        const j = Math.floor((currentY / this.canvas.height) * N) + 1;

        if (i >= 1 && i <= N && j >= 1 && j <= N) {
          u_prev[IX(i, j)] += dx*10; // Adjust the multiplier for sensitivity
          v_prev[IX(i, j)] += dy*10; // Adjust the multiplier for sensitivity
        }

        // Update last positions
        this.lastX = currentX;
        this.lastY = currentY;
      }
    });

    this.canvas.addEventListener('mouseup', () => {
      this.isDragging = false;
    });

    this.canvas.addEventListener('mouseleave', () => {
      this.isDragging = false;
    });
  }
}
const userInteraction = new UserInteraction(canvas);