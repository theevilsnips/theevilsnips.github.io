//2d Billiards Simulation

const canvas = document.getElementById('canvas');
const ctx = canvas.getContext('2d');
class Vector2D {
  constructor(x=0, y=0) {
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
class Ball {
  constructor(x, y, radius, color, vel=new Vector2D(), mass=1) {
    this.pos = new Vector2D(x, y);
    this.radius = radius;
    this.color = color;
    this.mass = mass;
    this.vel = vel
  }

  draw() {
    ctx.beginPath();
    ctx.arc(this.pos.x, this.pos.y, this.radius, 0, Math.PI * 2);
    ctx.fillStyle = this.color;
    ctx.fill();
    ctx.closePath();
    //console.log("Draw")
  }
  draw_debug(){
    ctx.beginPath();
    ctx.moveTo(this.pos.x,this.pos.y);
    ctx.lineTo(this.pos.x+10*this.vel.x,this.pos.y+10*this.vel.y);
    ctx.stroke();
    ctx.closePath();
    return;
  }
  handle_collision(other, restitution) {
    
    if (!collisions_enabled) return; // Skip collision handling if disabled
    //if (this.vel.dot_product(other.pos.add(this.pos.multiply(-1)).unit_vector()) < 0) return; // Skip if moving away
    const normal=other.pos.add(this.pos.multiply(-1));
    
    const normal_length = normal.length();
    if (normal_length > this.radius + other.radius) {return;}; // No collision
    let v_1=this.vel;
    let v_2=other.vel;
    const m_1=this.mass;
    const m_2=other.mass;
    
    const u_normal=normal.unit_vector();
    const u_tangential=new Vector2D(-u_normal.y,u_normal.x);
    const v1n=u_normal.dot_product(v_1);
    const v2n=u_normal.dot_product(v_2);
    const v1t=u_tangential.dot_product(v_1);
    const v2t=u_tangential.dot_product(v_2);
    const v1np=u_normal.multiply((v1n*(m_1-m_2)+2*m_2*v2n)/(m_1+m_2));
    const v2np=u_normal.multiply((v2n*(m_2-m_1)+2*m_1*v1n)/(m_2+m_1));
    const correction=(this.radius+other.radius-normal_length)/2;
    this.pos=this.pos.add(normal.multiply(-correction));
    other.pos=other.pos.add(normal.multiply(correction));
    this.vel = v1np.add(v1t);
    other.vel = v2np.add(v2t);
    console.log("Collision")
    
  }
  update() {
    
    if (gravity_enabled) {
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
    for (const ball of balls) {
      if (ball !== this) {
        this.handle_collision(ball, restitution);

      }
    }
  }
}
const gravity = 0.1; // Gravity acceleration
let restitution = 0.8; // Coefficient of restitution for collisions
let gravity_enabled = true; // Flag to toggle gravity
let collisions_enabled = true; // Flag to toggle collisions
function init() {
  const balls = [];
  for (let i = 0; i < 10; i++) {
    const radius = 20;
    const x = Math.random() * (canvas.width - radius * 2) + radius;
    const y = Math.random() * (canvas.height - radius * 2) + radius;
    const color = `hsl(${Math.random() * 360}, 100%, 50%)`;
    const velocity=new Vector2D(4*(2*Math.random()-1),4*(2*Math.random()-1));
    balls.push(new Ball(x, y, radius, color, velocity));
  }
}; init();
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
  gravity_enabled = !gravity_enabled; // Toggle gravity on/off
});
document.getElementById('restitutionSlider').addEventListener('input', (event) => {
  restitution = parseFloat(event.target.value);
});
let debug_text=document.getElementById("debug");
