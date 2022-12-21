const TILE_SIZE = 32;
const MAP_NUM_ROWS = 11;
const MAP_NUM_COLS = 15;

const WINDOW_WIDTH = MAP_NUM_COLS * TILE_SIZE;
const WINDOW_HEIGHT = MAP_NUM_ROWS * TILE_SIZE;

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
        //Origin first then projected point from trig.
        line(
            this.x,
            this.y,
            this.x + Math.cos(this.rotationAngle) * 30,
            this.y + Math.sin(this.rotationAngle) * 30
        );
    }
}

var grid = new Map();
var player = new Player();

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

function setup(){
    //Initialize all objects
    createCanvas(WINDOW_WIDTH, WINDOW_HEIGHT);

}

function update(){
    //Update all game objects before we render the next frame
    player.update();

}

function draw(){
    update();
    //render all objects frame by frame
    grid.render();
    player.render();
}
