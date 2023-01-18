const TILE_SIZE = 32;
const MAP_NUM_ROWS = 11;
const MAP_NUM_COLS = 15;

const WINDOW_WIDTH = MAP_NUM_COLS * TILE_SIZE;
const WINDOW_HEIGHT = MAP_NUM_ROWS * TILE_SIZE;

//field of view angle in radians
const FOV_ANGLE = 60 * (Math.PI / 180);

//Width of strip and number of arrays for castAllRays function.
const WALL_STRIP_WIDTH = 30;
const NUM_RAYS = WINDOW_WIDTH / WALL_STRIP_WIDTH; 

class Map{
    constructor(){
        //zeroes are empty ones are walls
        this.grid = [
            [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1],
            [1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1],
            [1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 1],
            [1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1],
            [1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1],
            [1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 1],
            [1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1],
            [1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1],
            [1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 0, 1],
            [1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1],
            [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1]
        ];
    }

    hasWallAt(x, y) {
        if(x < 0 || x > WINDOW_WIDTH || y < 0 || y > WINDOW_HEIGHT){
            return true;
        }


        //translate the player's screen position to equivalent map cell
        var mapX = Math.floor(x / TILE_SIZE);
        var mapY = Math.floor(y / TILE_SIZE);

        if (this.grid[mapY][mapX] == 0) {
            return false;
        }
        return true;
    }

    
    render(){
        for(var i = 0; i < MAP_NUM_ROWS; i++){
            for (var j = 0; j < MAP_NUM_COLS; j++){
                var tileX = j * TILE_SIZE;
                var tileY = i * TILE_SIZE;
                var tileColor = this.grid[i][j] == 1 ? "#222" : "#fff";
                stroke("#222");
                fill(tileColor);
                rect(tileX, tileY, TILE_SIZE, TILE_SIZE);
            }
        }
    }
}

class Player{

    constructor(){
        //start at the middle.
        this.x = WINDOW_WIDTH / 2;
        this.y = WINDOW_HEIGHT / 2;
        this.radius = 3;
        this.turnDirection = 0; //-1 if left, +1 if right
        this.walkDirection = 0; //-1 if going back, +1 if going forwards
        this.rotationAngle = Math.PI / 2;
        this.moveSpeed = 2.0;
        this.rotationSpeed = 2 * (Math.PI / 180);    //change two degrees.
    }

    update(){
        //update player positon based on turn and walk direction.
        this.rotationAngle += this.turnDirection * this.rotationSpeed;

        var moveStep = this.walkDirection * this.moveSpeed;
        var newPlayerX = this.x + Math.cos(this.rotationAngle) * moveStep;
        var newPlayerY = this.y + Math.sin(this.rotationAngle) * moveStep;

        if(!grid.hasWallAt(newPlayerX, newPlayerY)){
            this.x = newPlayerX;
            this.y = newPlayerY;
        }
    }

    render(){
        noStroke();
        fill("red");
        circle(this.x, this.y, this.radius);
        stroke("red");
        // Origin first then projected point from trig.
        line(
            this.x,
            this.y,
            this.x + Math.cos(this.rotationAngle) * 30,
            this.y + Math.sin(this.rotationAngle) * 30
        );
    }
}

class Ray{
    constructor(rayAngle){
        //Keep the angle between 0 and 2pi with normalizeAngle
        this.rayAngle = normalizeAngle(rayAngle);
        //location of wall hit location and distance of ray from player to wall.
        this.wallHitX = 0;
        this.wallHitY = 0;
        this.distance = 0;
        this.wasHitVertical = false;

        //Checks of ray location.
        this.isRayFacingDown = this.rayAngle > 0 && this.rayAngle < Math.PI;
        this.isRayFacingUp = !this.isRayFacingDown;

        this.isRayFacingRight = this.rayAngle > 1.5 * Math.PI || this.rayAngle < 0.5 * Math.PI;
        this.isRayFacingLeft = !this.isRayFacingRight;
    }

    cast(columnId){
        var xstep, ystep;           //deltax and deltay
        var xintercept, yintercept; //where ray collides with wall

        //HORIZONTAL RAY-GRID INTERSECTION CODE

        // console.log("isRayFacingRight?", this.isRayFacingRight);

        var foundHorizWallHit = false;
        var horizWallHitX = 0;
        var horizWallHitY = 0;

        //Find y coordinate of closest horiz. grid intersection, then x coordinate of horiz
        //grid intersect. Add tile size if ray facing down
        yintercept = Math.floor(player.y / TILE_SIZE) * TILE_SIZE;
        yintercept += this.isRayFacingDown ? TILE_SIZE : 0;

        xintercept = player.x + (yintercept - player.y) / Math.tan(this.rayAngle);

        ystep = TILE_SIZE;
        //invert if up or don't do anything.
        ystep *= this.isRayFacingUp ? -1 : 1;

        //left and x positive invert, right and x negative invert, otherwise don't do anything.
        xstep = TILE_SIZE / Math.tan(this.rayAngle)
        xstep *= (this.isRayFacingLeft && xstep > 0) ? -1 : 1
        xstep *= (this.isRayFacingRight && xstep < 0) ? -1 : 1
        
        var nextHorizTouchX = xintercept;
        var nextHorizTouchY = yintercept;

        //Force the horizontal touch X to be in the next tile by one pixel.
        if(this.isRayFacingUp){
            nextHorizTouchY--;
        }

        //increment xstep and ystep until wall is found or outside of map.
        while(nextHorizTouchX >= 0 && nextHorizTouchX <= WINDOW_WIDTH && nextHorizTouchY >= 0 && nextHorizTouchY <= WINDOW_HEIGHT){
            if(grid.hasWallAt(nextHorizTouchX, nextHorizTouchY)){
                //we found a wall hit
                foundHorizWallHit = true;
                horizWallHitX = nextHorizTouchX;
                horizWallHitY = nextHorizTouchY;
                break;
            }
            else{
                //increment xstep and ystip
                nextHorizTouchX += xstep;
                nextHorizTouchY += ystep;
            }
        }
        
        //VERTICAL RAY-GRID INTERSECTION CODE

        // console.log("isRayFacingRight?", this.isRayFacingRight);

        var foundVertWallHit = false;
        var vertWallHitX = 0;
        var vertWallHitY = 0;

        //Find x coordinate of closest vert. grid intersection, then y coordinate of vert
        //grid intersect. Add tile size if ray facing right
        xintercept = Math.floor(player.x / TILE_SIZE) * TILE_SIZE;
        xintercept += this.isRayFacingLeft ? TILE_SIZE : 0;

        yintercept = player.y + (xintercept - player.x) * Math.tan(this.rayAngle);

        xstep = TILE_SIZE;
        //invert if left or don't do anything.
        xstep *= this.isRayFacingLeft ? -1 : 1;

        //Up and y positive invert, down and y negative invert, otherwise don't do anything.
        ystep = TILE_SIZE * Math.tan(this.rayAngle)
        ystep *= (this.isRayFacingUp && ystep > 0) ? -1 : 1
        ystep *= (this.isRayFacingDown && ystep < 0) ? -1 : 1
        
        var nextVertTouchX = xintercept;
        var nextVertTouchY = yintercept;

        //Force the vertical touch X to be in the next tile by one pixel.
        if(this.isRayFacingLeft){
            nextVertTouchX--;
        }

        //increment xstep and ystep until wall is found or outside of map.
        while(nextVertTouchX >= 0 && nextVertTouchX <= WINDOW_WIDTH && nextVertTouchY >= 0 && nextVertTouchY <= WINDOW_HEIGHT){
            if(grid.hasWallAt(nextVertTouchX, nextVertTouchY)){
                //we found a wall hit
                foundVertWallHit = true;
                vertWallHitX = nextVertTouchX;
                vertWallHitY = nextVertTouchY;
                break;

            }
            else{
                //increment xstep and ystep
                nextVertTouchX += xstep;
                nextVertTouchY += ystep;
            }
        }

        //Calculate both horiz and vert distances and choose the smallest value.
        var horizHitDistance = (foundHorizWallHit) 
            ? distanceBetweenPoints(player.x, player.y, horizWallHitX, horizWallHitY)
            : Number.MAX_VALUE;

        var vertHitDistance = (foundVertWallHit)
            ? distanceBetweenPoints(player.x, player.y, vertWallHitX, vertWallHitY)
            : Number.MAX_VALUE;
        
        //store only the smallest of the distances
        this.wallHitX = (horizHitDistance < vertHitDistance) ? horizWallHitX : vertWallHitX;
        this.wallHitY = (horizHitDistance < vertHitDistance) ? horizWallHitY : vertWallHitY;
        this.distance = (horizHitDistance < vertHitDistance) ? horizHitDistance : vertHitDistance;
        this.wasHitVertical = (vertHitDistance < horizHitDistance);
    }

    render(){
        stroke("rgba(255, 0, 0, 0.3)");
        //line extends from origin to projected x and y of the ray using trig!
        line(
            player.x, 
            player.y, 
            this.wallHitX,
            this.wallHitY
        );

    }
}

var grid = new Map();
var player = new Player();
var rays = [];

//p5js listener function
function keyPressed(){
    if(keyCode == UP_ARROW){
        player.walkDirection = +1;
    }
    else if(keyCode == DOWN_ARROW){
        player.walkDirection = -1;
    }
    else if(keyCode == RIGHT_ARROW){
        player.turnDirection = +1;
    }
    else if(keyCode == LEFT_ARROW){
        player.turnDirection = -1;
    }
    
}

function keyReleased(){
    if(keyCode == UP_ARROW){
        player.walkDirection = 0;
    }
    else if(keyCode == DOWN_ARROW){
        player.walkDirection = 0;
    }
    else if(keyCode == RIGHT_ARROW){
        player.turnDirection = 0;
    }
    else if(keyCode == LEFT_ARROW){
        player.turnDirection = 0;
    }
}

function castAllRays(){
    var columnId = 0;

    //start at first ray (ANGLE - HALF OF ANGLE)
    var rayAngle = player.rotationAngle - (FOV_ANGLE / 2);

    rays = [];

    //loop all columns casting the rays
    for(var i = 0; i < NUM_RAYS; i++){
        var ray = new Ray(rayAngle);
        ray.cast(columnId);

        //place the ray in the rays array and increment angle and column id.
        rays.push(ray);
        rayAngle += FOV_ANGLE / NUM_RAYS;
        columnId++;
    }
}

function normalizeAngle(angle){
    //keep angle between 0 and 2pi
    angle = angle % (2 * Math.PI)
    //keep angle positive.
    if(angle < 0){
        angle = (2 * Math.PI) + angle;
    }
    return angle;
}

function distanceBetweenPoints(x1, y1, x2, y2){
    return Math.sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
}

function setup(){
    //Initialize all objects
    createCanvas(WINDOW_WIDTH, WINDOW_HEIGHT);

}

function update(){
    //Update all game objects before we render the next frame
    player.update();
    castAllRays();

}

function draw(){
    update();
    //render all objects frame by frame
    grid.render();
    for(ray of rays){   //for each ray of rays.
        ray.render();
    }
    player.render();
    // castAllRays();
}
