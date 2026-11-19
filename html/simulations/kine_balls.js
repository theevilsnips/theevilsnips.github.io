const canvas = document.getElementById('canvas');
const ctx = canvas.getContext('2d');
class Vector2D {
  constructor(x, y) {
    this.x = x;
    this.y = y;
  }
  length() {
    return Math.sqrt(this.x * this.x + this.y * this.y);
  }

  normalize() {
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
  dotProduct(vector) {
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
class Ball {
  constructor(x, y, radius, color, mass = 1) {
    this.pos = new Vector2D(x, y);
    this.radius = radius;
    this.color = color;
    this.mass = mass;
    this.vel = new Vector2D(Math.random() * 4 - 2, Math.random() * 4 - 2); // Random velocity
  }

  draw() {
    ctx.beginPath();
    ctx.arc(this.pos.x, this.pos.y, this.radius, 0, Math.PI * 2);
    ctx.fillStyle = this.color;
    ctx.fill();
    ctx.closePath();
  }
  draw_debug(){
    debug_text.innerText=`Debug`;
    ctx.strokeStyle="#000000";
    ctx.lineWidth=10;
    ctx.moveTo(this.pos.x,this.pos.y);
    ctx.lineTo(this.pos.x+10*this.vel.x,this.pos.y+10*this.vel.y);
  }
  handleCollision(other, restitution) {
    if (!collisionsEnabled) return; // Skip collision handling if disabled
    const dir= new Vector2D(this.pos.x - other.pos.x, this.pos.y - other.pos.y);
    let dir_length = dir.length();
    if (dir_length > this.radius + other.radius) return; // No collision
    dir.normalize();
    const correction = (this.radius + other.radius - dir_length) / 2;
    this.pos = this.pos.add(dir.multiply(correction));
    other.pos = other.pos.add(dir.multiply(-correction));
    const v1 = this.vel.dotProduct(dir);
    const v2 = other.vel.dotProduct(dir);
    const m1 = this.mass;
    const m2 = other.mass;
    const new_v1 = (m1 * v1 + m2 * v2 - m2 * (v1 - v2) * restitution) / (m1 + m2);
	  const new_v2 = (m1 * v1 + m2 * v2 - m1 * (v2 - v1) * restitution) / (m1 + m2);
    this.vel = this.vel.add(dir.multiply(new_v1 - v1));
    other.vel = other.vel.add(dir.multiply(new_v2 - v2));
  }
  update() {
    /*for (const ball of balls) {
      if (ball !== this) {
        this.handleCollision(ball, restitution);
      }
    }*/
    if (gravityEnabled) {
      this.vel = this.vel.add(new Vector2D(0, gravity)); // Apply gravity to velocity
    }
    this.pos = this.pos.add(this.vel);
    
    // Bounce off the walls
    if (this.pos.x + this.radius > canvas.width || this.pos.x - this.radius < 0) {
      this.vel = new Vector2D(-this.vel.x, this.vel.y);
    }
    if (this.pos.y + this.radius > canvas.height || this.pos.y - this.radius < 0) {
      this.vel = new Vector2D(this.vel.x, -this.vel.y);
    }
    
  }
}
const gravity = 0.1; // Gravity acceleration
let restitution = 0.8; // Coefficient of restitution for collisions
let gravityEnabled = true; // Flag to toggle gravity
let collisionsEnabled = true; // Flag to toggle collisions
const balls = [];
for (let i = 0; i < 10; i++) {
  const radius = 20;
  const x = Math.random() * (canvas.width - radius * 2) + radius;
  const y = Math.random() * (canvas.height - radius * 2) + radius;
  const color = `hsl(${Math.random() * 360}, 100%, 50%)`;
  balls.push(new Ball(x, y, radius, color));
}

function animate() {
  if (isRunning) {
  requestAnimationFrame(animate);
    ctx.clearRect(0, 0, canvas.width, canvas.height);
  
  for (const ball of balls) {
    ball.update();
    
    ball.draw();
    ball.draw_debug();
  }
  
}
}
let isRunning = true;
animate();

document.getElementById('startButton').addEventListener('click', () => {
  if (!isRunning) {
    isRunning = true;
    requestAnimationFrame(animate);
  }
});

document.getElementById('stopButton').addEventListener('click', () => {
  isRunning = false;
  cancelAnimationFrame(animate);
});

document.getElementById('resetButton').addEventListener('click', () => {
  balls.length = 0; // Clear the balls array
  for (let i = 0; i < 10; i++) {
    const radius = 20;
    const x = Math.random() * (canvas.width - radius * 2) + radius;
    const y = Math.random() * (canvas.height - radius * 2) + radius;
    const color = `hsl(${Math.random() * 360}, 100%, 50%)`;
    balls.push(new Ball(x, y, radius, color));
  }
});
document.getElementById('gravityToggleButton').addEventListener('click', () => {
  gravityEnabled = !gravityEnabled; // Toggle gravity on/off
});
document.getElementById('restitutionSlider').addEventListener('input', (event) => {
  restitution = parseFloat(event.target.value);
});
let debug_text=document.getElementById("debug");
