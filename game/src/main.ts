import P5 from "p5";

type Paddle = {
  score: number;
  angle: number;
  x: number;
  y: number;
  height: number;
  width: number;
};

type Ball = {
  x: number;
  radius: number;
  y: number;
};

type Game = {
  width: number;
  height: number;
  left: Paddle;
  right: Paddle;
  ball: Ball;
};

let game: Game = {
  width: 400,
  height: 400,
  left: { height: 10, width: 10, score: 10, angle: 68, x: 100, y: 100 },
  right: { height: 10, width: 10, score: 0, angle: 33, x: 200, y: 300 },
  ball: {
    x: 100,
    y: 100,
    radius: 10,
  },
};

function drawPaddle(p5: P5, paddle: Paddle) {
  p5.push();

  p5.translate(paddle.x, paddle.y);
  p5.angleMode("degrees");
  p5.rotate(paddle.angle + 90);

  p5.noStroke();
  p5.fill("white");
  p5.rectMode("center");
  p5.rect(0, 0, paddle.width, paddle.height);

  p5.pop();
}

function drawBall(p5: P5, ball: Ball) {
  p5.fill("white");
  p5.rectMode("center");
  p5.rect(ball.x, ball.y, ball.radius);
}

function drawCenterLine(p5: P5) {
  const lineHeight = 20;
  const lineWidth = 6;
  const lineSpacing = 5;

  let walk = 0;
  while (walk < p5.height) {
    p5.rect(p5.width / 2 - lineWidth / 2, walk, lineWidth, lineHeight);
    walk += lineHeight + lineSpacing;
  }
}

function drawScores(p5: P5, leftScore: number, rightScore: number) {
  const yOffset = 10;
  const xOffsetFromCenter = 50;
  p5.textAlign("center", "top");
  p5.textSize(50);
  p5.text(leftScore.toString(), p5.width / 2 - xOffsetFromCenter, yOffset);
  p5.text(rightScore.toString(), p5.width / 2 + xOffsetFromCenter, yOffset);
}

let socket: WebSocket;
let started = false;

const sketch = (p5: P5) => {
  p5.setup = () => {
    const canvas = p5.createCanvas(400, 400);
    canvas.parent("container");

    p5.background("black");

    socket = new WebSocket(
      "ws://127.0.0.1:7681",
      "lws-tiltpong-send-game-data",
    );
    socket.addEventListener("message", (data) => {
      try {
        game = JSON.parse(data.data);
      } catch {}
    });
  };

  p5.draw = () => {
    p5.background("black");
    drawPaddle(p5, game.left);
    drawPaddle(p5, game.right);
    drawScores(p5, game.left.score, game.right.score);
    drawCenterLine(p5);
    drawBall(p5, game.ball);

    if (!started && p5.keyIsPressed == true && p5.key == " ") {
      socket.send("start");
      started = true;
    }
  };
};

new P5(sketch);
