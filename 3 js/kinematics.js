import * as three from "three";

const scene = new three.Scene();
scene.background = new three.Color( 0x202020 );
const camera = new three.PerspectiveCamera( 75, window.innerWidth / window.innerHeight, 0.1, 1000 );

const renderer = new three.WebGLRenderer();
renderer.setSize( window.innerWidth, window.innerHeight );
document.body.appendChild( renderer.domElement );
const geometry = new three.TorusGeometry( 1, 0.3, 16, 100 );
const material = new three.MeshPhongMaterial( { color: 0xffff00 } );
class kine_object extends three.Mesh {
    constructor(geometry, material, velocity = new three.Vector3(), angular_velocity = new three.Vector3()) {
        super(geometry, material);
        this.velocity = velocity;
        this.angular_velocity = angular_velocity;
    }
};
const tori = [];
for (let i = 0; i < 10; i++) {
    const torus = new kine_object( geometry, material );
    torus.position.x = Math.random() * 10 - 5;
    torus.position.y = Math.random() * 10 - 5;
    torus.position.z = Math.random() * 10 - 5;
    torus.velocity.x = Math.random() * 2 - 1;
    torus.velocity.y = Math.random() * 2 - 1;
    torus.velocity.z = Math.random() * 2 - 1;
    scene.add( torus );
    tori.push(torus);
}

const light = new three.PointLight( 0xffffff, 1000, 0 );
light.position.set( 50, 50, 50 );
scene.add( light );
const acc_g=-9.81;
const time_step = 0.01;
camera.position.z = 10;
function obj_kinematics(object) {
    if (object instanceof kine_object) {
        // Apply kinematic updates
        object.position.add((object.velocity.clone().add(new three.Vector3(0, acc_g, 0).multiplyScalar(0.5))).multiplyScalar(time_step));
        object.velocity.add(new three.Vector3(0, acc_g, 0).multiplyScalar(time_step)); // Gravity effect
        if (object.position.y < -5) {
            object.position.y = 5;
        }
    }
};
function animate(time) {
    
    tori.forEach(torus => {
        obj_kinematics(torus);
    });
    renderer.render( scene, camera );
};
    
renderer.setAnimationLoop( animate );