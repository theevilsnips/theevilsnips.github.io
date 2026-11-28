//2d Billiards Simulation

const canvas = document.getElementById('canvas');
const ctx = canvas.getContext('2d');

class Vector2D {
  constructor(x = 0, y = 0) {
    this.x = x;
    this.y = y;
  }
  length() {
    return Math.sqrt(this.x * this.x + this.y * this.y);
  }
  unit_vector() {
    const len = this.length();
    if (len > 0) {
      return new Vector2D(this.x / len, this.y / len);
    }
    return new Vector2D(1, 0);
  }
  add(vector) {
    return new Vector2D(this.x + vector.x, this.y + vector.y);
  }
  multiply(scalar) {
    return new Vector2D(this.x * scalar, this.y * scalar);
  }
  dot_product(vector) {
    return this.x * vector.x + this.y * vector.y;
  }
  set(x, y) {
    this.x = x;
    this.y = y;
  }
  clone() {
    return new Vector2D(this.x, this.y);
  }
}

const gravity = 0.1;            // Gravity acceleration (per frame)
let restitution = 0.8;          // Coefficient of restitution
let gravity_enabled = true;
let collisions_enabled = true;
let balls = [];
let ball_ids= [];
let dt=0;

class Ball {
  constructor(x, y, radius, color, id, vel = new Vector2D(), mass = 1) {
    this.pos = new Vector2D(x, y);
    this.radius = radius;
    this.color = color;
    this.mass = mass;
    this.vel = vel;
    this.is_pinned=false;
    this.id=id;
  }

  draw() {
    ctx.beginPath();
    ctx.arc(this.pos.x, this.pos.y, this.radius, 0, Math.PI * 2);
    ctx.fillStyle = this.color;
    ctx.fill();
    ctx.closePath();
  }

  draw_debug() {
    ctx.beginPath();
    ctx.moveTo(this.pos.x, this.pos.y);
    ctx.lineTo(this.pos.x + 10 * this.vel.x, this.pos.y + 10 * this.vel.y);
    ctx.stroke();
    ctx.closePath();
  }

  // Integrate only. Collisions are handled separately, once per pair.
  move() {
    if (this.is_pinned) return;
    if (gravity_enabled) {
      this.vel = this.vel.add(new Vector2D(0, gravity).multiply(60*dt));
    }
    this.pos = this.pos.add(this.vel.multiply(60*dt));
  }

  // Clamp position back inside the canvas and reflect velocity only if it
  // is heading into the wall (prevents getting stuck / flip-flopping).
  handle_walls() {
    const r = this.radius;
    if (this.pos.x - r < 0) {
      this.pos.x = r;
      if (this.vel.x < 0) this.vel.x *= -restitution;
    } else if (this.pos.x + r > canvas.width) {
      this.pos.x = canvas.width - r;
      if (this.vel.x > 0) this.vel.x *= -restitution;
    }
    if (this.pos.y - r < 0) {
      this.pos.y = r;
      if (this.vel.y < 0) this.vel.y *= -restitution;
    } else if (this.pos.y + r > canvas.height) {
      this.pos.y = canvas.height - r;
      if (this.vel.y > 0) this.vel.y *= -restitution;
    }
  }
}

function resolve_collision(a, b) {
  const delta = b.pos.add(a.pos.multiply(-1));
  const dist = delta.length();
  const min_dist = a.radius + b.radius;
  if (dist >= min_dist) return; // No overlap

  const n = delta.unit_vector();                 // collision normal (a -> b)
  const t = new Vector2D(-n.y, n.x);             // tangent
  const m1 = a.mass, m2 = b.mass;
  const total = m1 + m2;

  // Push the balls apart along the UNIT normal, weighted by mass
  const overlap = min_dist - dist;
  a.pos = a.pos.add(n.multiply(-overlap * m2 / total));
  b.pos = b.pos.add(n.multiply(overlap * m1 / total));

  const v1n = n.dot_product(a.vel);
  const v2n = n.dot_product(b.vel);
  if (v1n - v2n <= 0) return; // Already separating: don't touch velocities

  const v1t = t.dot_product(a.vel);
  const v2t = t.dot_product(b.vel);

  // 1D collision along the normal with coefficient of restitution
  const v1n_new = (m1 * v1n + m2 * v2n + m2 * restitution * (v2n - v1n)) / total;
  const v2n_new = (m1 * v1n + m2 * v2n + m1 * restitution * (v1n - v2n)) / total;

  // Tangential components are unchanged (frictionless); scalars must be
  // multiplied by the tangent vector before adding.
  a.vel = n.multiply(v1n_new).add(t.multiply(v1t));
  b.vel = n.multiply(v2n_new).add(t.multiply(v2t));
}
const id_gen= () => Math.floor(Math.random()*(2**32));

function init() {
  ball_ids =[];
  balls = [];
  const radius = 20;
  for (let i = 0; i < 10; i++) {
    // Try to spawn without overlapping existing balls
    let x, y, ok;
    for (let attempt = 0; attempt < 100; attempt++) {
      x = Math.random() * (canvas.width - radius * 2) + radius;
      y = Math.random() * (canvas.height - radius * 2) + radius;
      ok = balls.every(b => Math.hypot(b.pos.x - x, b.pos.y - y) >= b.radius + radius);
      if (ok) break;
    }
    const color = `hsl(${Math.random() * 360}, 100%, 50%)`;
    const velocity = new Vector2D(4 * (2 * Math.random() - 1), 4 * (2 * Math.random() - 1));
    const id=id_gen();
    ball_ids.push(id);
    balls.push(new Ball(x, y, radius, color, id, velocity));
  }
}

function render() {
  ctx.clearRect(0, 0, canvas.width, canvas.height);
  for (const ball of balls) {
    ball.draw();
    //ball.draw_debug();
  }
}

function step() {
  for (const ball of balls) ball.move();
  for (const ball of balls) ball.handle_walls();

  if (collisions_enabled) {
    for (let i = 0; i < balls.length; i++) {
      for (let j = i + 1; j < balls.length; j++) {
        resolve_collision(balls[i], balls[j]);
      }
    }
    // Collision push-out may have shoved a ball into a wall
    for (const ball of balls) ball.handle_walls();
  }
}

let animationId = null;
let time_last=0;
function animate(time) {
  animationId = requestAnimationFrame(animate);
  dt=(time-time_last)*0.001;
  //console.log(dt);
  step();
  render();
  time_last=time;
}

function start() {
  if (animationId === null) animationId = requestAnimationFrame(animate);
}

function stop() {
  if (animationId !== null) {
    cancelAnimationFrame(animationId);
    animationId = null;
  }
}

init();
start();
function find_ball_by_xy(x,y) {
  let ball;
  for (ball of balls) {
    if (ball.pos.add(new Vector2D(-x,-y)).length()<ball.radius) return ball;
  }
  return null
}
class UserInteraction {
  constructor(canvas) {
    this.canvas = canvas;
    this.isDragging = false;
    this.lastX = 0;
    this.lastY = 0;
    this.ball;
    this.canvas.addEventListener('mousedown', (e) => {
      const rect = this.canvas.getBoundingClientRect();
      this.isDragging = true;
      this.lastX = e.clientX - rect.left;
      this.lastY = e.clientY - rect.top;
      this.ball=find_ball_by_xy(this.lastX,this.lastY);
      if (this.ball !=null) {
      this.ball.is_pinned=true;
      } else {
        const id=id_gen();
        ball_ids.push(id);
        balls.push(new Ball(this.lastX,this.lastY, 20, `hsl(${Math.random() * 360}, 100%, 50%)`, id));
      }
    });

    this.canvas.addEventListener('mousemove', (e) => {
      if (this.isDragging) {
        const rect = this.canvas.getBoundingClientRect();
        const currentX = e.clientX - rect.left;
        const currentY = e.clientY - rect.top;

        // Calculate the change in position
        const dx = currentX - this.lastX;
        const dy = currentY - this.lastY;
       
        
        //console.log("Drag");
        if (this.ball==null) return;
        
        this.ball.pos=new Vector2D(currentX,currentY);
        this.ball.vel=new Vector2D(dx,dy);
        this.lastX=currentX;
        this.lastY=currentY;
      }
    });

    this.canvas.addEventListener('mouseup', () => {
      this.isDragging = false;
      if (this.ball==null) return;
      this.ball.is_pinned = false;
    });

    this.canvas.addEventListener('mouseleave', () => {
      this.isDragging = false;
      if (this.ball==null) return;
      this.ball.is_pinned = false;
    });

    this.canvas.addEventListener("auxclick", () => {
      const rect = this.canvas.getBoundingClientRect();
      
      this.ball=find_ball_by_xy(this.lastX,this.lastY);
      if (this.ball==null) return;
      const id=ball_ids.indexOf(this.ball.id);
      ball_ids.splice(id,1);
      balls.splice(id,1);
    });
  }
}
const userInteraction = new UserInteraction(canvas);

document.getElementById('startButton').addEventListener('click', start);
document.getElementById('stopButton').addEventListener('click', stop);
document.getElementById('resetButton').addEventListener('click', () => {
  init();
  render(); // so reset is visible even while stopped
});
document.getElementById('gravityToggleButton').addEventListener('click', () => {
  gravity_enabled = !gravity_enabled;
});
document.getElementById('restitutionSlider').addEventListener('input', (event) => {
  restitution = parseFloat(event.target.value);
});
let debug_text = document.getElementById("debug");