import P5 from "p5";

type Paddle = {
  score: number;
  angle: number;
  y: number;
}

type Ball = {
  x: number;
  y: number;
};

type Game = {
  paddles: [Paddle, Paddle];
  ball: Ball;
}

const game: Game = {
  paddles: [
    { score: 10, angle: 68, y: 100 },
    { score: 0, angle: 33, y: 300 },
  ],
  ball: {
    x: 100,
    y: 100,
  }
}

function drawPaddle(p5: P5, paddle: Paddle, playerNum: 0 | 1) {
  const offset = 50;
  const x = playerNum === 0 ? offset : p5.width - offset;

  p5.push();

  p5.rectMode("center");
  p5.translate(x, paddle.y);
  p5.angleMode("degrees");
  p5.rotate(paddle.angle);

  p5.noStroke();
  p5.fill("white")
  p5.rect(0, 0, 15, 100);

  p5.pop();
}

function drawBall(p5: P5, ball: Ball) {
  p5.fill("white");
  p5.rect(ball.x, ball.y, 15, 15);
}

function drawCenterLine(p5: P5) {
  const lineHeight = 20;
  const lineWidth = 6;
  const lineSpacing = 5;
  
  let walk = 0;
  while (walk < p5.height) {
    p5.rect(p5.width/2-lineWidth/2, walk, lineWidth, lineHeight);
    walk += lineHeight + lineSpacing;
  }
}

function drawScores(p5: P5, leftScore: number, rightScore: number) {
  const yOffset = 10;
  const xOffset = 250;
  p5.textAlign("center", "top");
  p5.textSize(50);
  p5.text(leftScore.toString(), xOffset ,yOffset);
  p5.text(rightScore.toString(), p5.width-xOffset, yOffset);
}

const sketch = (p5: P5) => {
  p5.setup = () => {
    const canvas = p5.createCanvas(800, 600);
    canvas.parent("container");

    p5.background("black");
  }

  p5.draw = () => {
    p5.background("black");
    drawPaddle(p5, game.paddles[0], 0);
    drawPaddle(p5, game.paddles[1], 1);
    drawScores(p5, game.paddles[0].score, game.paddles[1].score);
    drawCenterLine(p5);
    drawBall(p5, game.ball);
  }
}

new P5(sketch);
