#!/usr/bin/python
#
#------------------------------------------------------------------------------ 
# Pong!
#     No score limit, just keep playing!
#
# Controls:
#             P1: W - up
#                 S - down
#             P2: UP - up
#                 DOWN - down
#             Quit: ESC or Window 'X'
#
#------------------------------------------------------------------------------
#
# Written with Python grammar version 2.6.1
#
#------------------------------------------------------------------------------ 
#
# 2010 Aug 22 - Nick T. Lange: Initial creation
# 2010 Aug 23 - NL: Fixed collision angle, added support for simultaneous key presses
#
# TODO:
#    Add AI for player 2
#    Add menu for single/multi-player
#    Look into network play
#
#------------------------------------------------------------------------------ 

try:
    import pygame
    from pygame.locals import *
except:
    print 'ERROR: Pygame could not be imported, please check your installation'
    raise SystemExit
import sys
from math import cos,pi,sin,sqrt
from random import random

class board():
    def __init__(self,size_x,size_y):
        self.objects = []
        self.setBackground(size_x,size_y)
        self.setBorder(size_x/2,size_y)
    def setBackground(self,size_x,size_y):
        back = board.background(size_x,size_y)
        self.addObjects(back)
    def setBorder(self,x,height,width=2):
        borderline = board.border(x,height)
        self.addObjects(borderline)
    def addObjects(self,*objects):
        for object in objects:
            self.objects.append(object)
    def draw(self,screen):
        [object.draw(screen) for object in self.objects if hasattr(object,'draw')]

    class background():
        def __init__(self,size_x,size_y,colour=(0,0,0)):
            self.colour = colour
            self.setRect(size_x,size_y)
        def getRect(self):
            return self.rect
        def setRect(self,size_x,size_y):
            self.rect = pygame.Rect((0,0),(size_x,size_y))
        def draw(self,screen):
            pygame.draw.rect(screen,self.colour,self.rect)

    class border():
        def __init__(self,x,height,width=2,colour=(255,255,255)):
            self.height = height
            self.width = width
            self.colour = colour
            self.rect = self.createRect(x,width,height)
        def getPos(self):
            return self.rect.centerx
        def setPos(self,x):
            self.rect.centerx = x
        def getHeight(self):
            return self.rect.height
        def setHeight(self,height):
            self.rect.height = height
        def getWidth(self):
            return self.rect.width
        def setWidth(self,width):
            self.rect.width = width
        def createRect(self,x,width,height):
            rect = pygame.Rect((0,0),(width,height))
            rect.centerx = x
            return rect
        def draw(self,screen):
            pygame.draw.rect(screen,self.colour,self.rect)

class paddle():
    default_width = 3
    default_height = 40
    default_speed = 6
    default_colour = (255,255,255)
    def __init__(self,x,y,width=default_width,height=default_height,speed=default_speed,colour=default_colour):
        self.speed = speed
        self.colour = colour
        self.rect = self.createRect(x,y,width,height)
    def getPos(self):
        return self.rect.center
    def setPos(self,x,y):
        self.rect.center = x,y
    def getTop(self):
        return self.rect.top
    def getBottom(self):
        return self.rect.bottom
    def getLeft(self):
        return self.rect.left
    def getRight(self):
        return self.rect.right
    def getSpeed(self):
        return self.speed
    def setSpeed(self,speed):
        self.speed = speed
    def createRect(self,x,y,width,height):
        rect = pygame.Rect((0,0),(width,height))
        rect.center = x,y
        return rect
    def getRect(self):
        return self.rect
    def setRect(self,x,y,width,height):
        self.rect = pygame.Rect((x,y),(width,height))
    def move(self,dir):
        # dir < 0 means move up, dir > 0 means move down
        self.rect.centery += self.speed*dir
    def reset(self,x,y,speed=default_speed):
        self.__init__(x,y,speed=speed)
    def draw(self,screen):
        pygame.draw.rect(screen,self.colour,self.rect)

class ball():
    default_dir = [1.0,0.0]
    default_speed = 4.0
    default_size = 12
    default_colour = (122,255,0)
    def __init__(self,x,y,dir=default_dir,speed=default_speed,speed_y=default_speed,size=default_size,colour=default_colour):
        self.dir = dir
        self.speed = speed
        self.size = size
        self.colour = colour
        self.rect = self.createRect(x,y)
    def getPos(self):
        return self.rect.center
    def setPos(self,x,y):
        self.rect.center = (x,y)
    def getDir(self):
        return self.dir
    def setDir(self,x_comp,y_comp):
        self.dir = [x_comp,y_comp]
    def reverseX(self):
        self.dir[0] *= -1
    def reverseY(self):
        self.dir[1] *= -1
    def createRect(self,x,y):
        rect = pygame.Rect((0,0),(self.size,self.size))
        rect.center = x,y
        return rect
    def getRect(self):
        return self.rect
    def setRect(self,x,y,width,height):
        self.rect = pygame.Rect((x,y),(width,height))
    def getTop(self):
        return self.rect.top
    def getBottom(self):
        return self.rect.bottom
    def getLeft(self):
        return self.rect.left
    def getRight(self):
        return self.rect.right
    def getSpeed(self):
        return self.speed
    def getSpeedX(self):
        return self.speed*self.dir[0]
    def getSpeedY(self):
        return self.speed*self.dir[1]
    def setSpeed(self,speed_x,speed_y):
        self.speed = sqrt(speed_x**2+speed_y**2)
    def incrementSpeed(self):
        self.speed += 1.0
    def move(self):
        self.rect.centerx += int(self.getSpeedX())
        self.rect.centery += int(self.getSpeedY())
    def calcDir(self,paddle):
        angle = 50.*pi/180.*(self.rect.centery-paddle.rect.centery)/(paddle.rect.height/2.)
        x_comp = cos(angle)
        y_comp = sin(angle)
        if self.getSpeedX() > 0:
            x_comp *= -1
        self.setDir(x_comp,y_comp)
    def reset(self,x,y,dir=default_dir,speed=default_speed,size=default_size,colour=default_colour):
        self.__init__(x,y,dir=dir,speed=speed,size=size,colour=colour)
    def draw(self,screen):
        pygame.draw.rect(screen,self.colour,self.rect)

class scoreCounter():
    default_size = 50
    default_colour = (255,255,255)
    def __init__(self,x,y,size=default_size,colour=default_colour):
        self.value = 0
        self.colour = colour
        self.setFont(size)
        self.setText()
        self.rect = self.createRect(x,y)
    def getColour(self):
        return self.colour
    def setColour(self,colour):
        self.colour = colour
    def getSize(self):
        return self.rect.height
    def setSize(self,size):
        self.rect.height,self.rect.width = size,size
    def setFont(self,size):
        self.font = pygame.font.Font('font.ttf',size)
    def getPos(self):
        return self.rect.center
    def setPos(self,x,y):
        self.rect.center = x,y
    def createRect(self,x,y):
        rect = self.text.get_rect()
        rect.center = x,y
        return rect
    def getRect(self):
        return self.rect
    def setRect(self,x,y,width,height):
        self.rect = pygame.Rect((x,y),(width,height))
    def setText(self):
        self.text = self.font.render('%d' % self.value,True,self.colour)
    def getValue(self):
        return self.value
    def setValue(self,value):
        self.value = value
    def incrementValue(self):
        self.value += 1
        self.update()
    def update(self):
        self.setText()
        self.rect = self.createRect(self.rect.centerx,self.rect.centery)
    def draw(self,screen):
        screen.blit(self.text,self.rect)

def wait(time):
    clock = pygame.time.Clock()
    clock.tick()
    while time > 0:
        for event in pygame.event.get():
            if event.type == QUIT or (event.type == KEYDOWN and event.key == K_ESCAPE):
                print 'Quitting!'
                pygame.quit()
                sys.exit()
        time -= clock.tick()

def aiDir(paddle,ball,screenHeight):
    # TODO: Add support for walls
    ball_x,ball_y = ball.getPos()
    ball_dir = ball.getDir()
    paddle_x,paddle_y = paddle.getPos()
    
    dir = 0
    try:
        new_y = -ball_dir[1]/ball_dir[0]*(paddle_x-ball_x)+ball_y
        if new_y > screenHeight or new_y < 0:
            new_y = new_y-(screenHeight/2.0)
        if new_y > paddle_y + paddle.getRect().height/2. or new_y < paddle_y - paddle.getRect().height/2.:
            dir = ball.getSpeedY()*(new_y-paddle_y)/(new_y-ball_y)/paddle.getSpeed()
    except ZeroDivisionError:
        return dir
    if dir > 1:
        dir = 1
    if dir < -1:
        dir = -1
    return dir

def main():
    res_x,res_y = 1024,768

    pygame.init()
    pygame.display.set_caption('Pong!')
    pygame.display.set_icon(pygame.image.load('icon.gif'))
    screen = pygame.display.set_mode((res_x,res_y),0,32)
    
    gameboard = board(res_x,res_y)
    b1 = ball(res_x/2,res_y/2)
    p1 = paddle(50,res_y/2-10)
    p2 = paddle(res_x-50,res_y/2+10)
    s1 = scoreCounter(res_x/4,res_y/12)
    s2 = scoreCounter(3*res_x/4,res_y/12)
    gameboard.addObjects(p1,p2,s1,s2,b1)
    
    clock = pygame.time.Clock()

    dir1,dir2 = 0,0
    hits = 0
    
    while True:
        # Check for victory conditions
        b1_left,b1_right = b1.getLeft(),b1.getRight()
        if b1_left < 0 or b1_right > res_x:
            if b1_left < 0:
                s2.incrementValue()
                b1.reset(res_x/2,res_y/2,dir=[-1.0,0.0])
            elif b1_right > res_x:
                s1.incrementValue()
                b1.reset(res_x/2,res_y/2,dir=[1.0,0.0])
            p1.reset(50,res_y/2-10)
            p2.reset(res_x-50,res_y/2+10)
            gameboard.draw(screen)
            pygame.display.update()
            dir1,dir2 = 0,0
            hits = 0
            wait(3000)
        
        # Get input from players
        for event in pygame.event.get():
            if event.type == QUIT or (event.type == KEYDOWN and event.key == K_ESCAPE):
                print 'Quitting!'
                pygame.quit()
                sys.exit()
            if event.type == KEYDOWN:
                if event.key == K_w:
                    dir1 = -1
                elif event.key == K_s:
                    dir1 = 1
                elif event.key == K_UP:
                    dir2 = -1
                elif event.key == K_DOWN:
                    dir2 = 1
            if event.type == KEYUP:
                if event.key in [K_w,K_s]:
                    dir1 = 0
                elif event.key in [K_UP,K_DOWN]:
                    dir2 = 0
        
        # Handle paddles at the edge of the board
        p1_top,p1_bottom = p1.getTop(),p1.getBottom()
        if (dir1 > 0  and p1_bottom > res_y) or (dir1 < 0  and p1_top < 0):
            dir1 = 0
        p2_top,p2_bottom = p2.getTop(),p2.getBottom()
        if (dir2 > 0  and p2_bottom > res_y) or (dir2 < 0  and p2_top < 0):
            dir2 = 0
        
        # Ball hits top/bottom wall
        b1_top,b1_bottom = b1.getTop(),b1.getBottom()
        if b1_top < 0 or b1_bottom > res_y:
            b1.reverseY()
        
        # Ball hits a paddle
        paddle_list = [p1,p2]
        collision = b1.getRect().collidelist([p.getRect() for p in paddle_list])
        if collision != -1:
            hits += 1
            curr_paddle = paddle_list[collision]
            b1.calcDir(curr_paddle)
        
        # Update entities
        p1.move(dir1)
        p2.move(dir2)
        b1.move()
        
        # Increase speed every 4 hits until a maximum is achieved
        if hits != 0 and hits % 3 == 0 and b1.getSpeed() <= 11:
            hits = 0
            b1.incrementSpeed()
        
        # Limit frame-rate to 60fps
        clock.tick(40)
        # Draw entities and update screen
        gameboard.draw(screen)
        pygame.display.update()

if __name__ == '__main__':
    main()
