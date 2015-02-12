var animator : Animator; //stores the animator component
var v : float; //vertical movements
var h : float; //horizontal movements
var run : float;
var jump : float;
 
function Start () {
 
animator = GetComponent(Animator); //assigns Animator component when we start the game
 
}
 
function Update () {
 
v = Input.GetAxis("Vertical");
h = Input.GetAxis("Horizontal");
Running();
Jumping();
 
}
 
function FixedUpdate () {
 
//set the "Walk" parameter to the v axis value
animator.SetFloat ("Walk", v);
animator.SetFloat ("Turn", h);
animator.SetFloat("Sprint", run);
animator.SetFloat("Jump", jump);
 
}
 
function Running () {
if(Input.GetButton("Fire1")) {
run = 0.2;
}
else {
run = 0.0;
}
 
}

function Jumping () {
if (Input.GetKeyDown ("space")) {
jump = 0.2;
}
else {
jump = 0.0;
}
 
}

