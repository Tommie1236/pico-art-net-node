#!/usr/bin/env python3

import pygame
import pprint
import time

DISP_WIDHT = 128
DISP_HEIGHT = 64
PIXEL_SIZE = 8

WINDOW_WIDTH = DISP_WIDHT * PIXEL_SIZE + 20
WINDOW_HEIGHT = DISP_HEIGHT * PIXEL_SIZE + 70

class pixel:
    def __init__(self, screen: pygame.Surface, x: int, y: int, size: tuple[int, int]):
        self.x: int = x
        self.y: int = y
        self.size: tuple[int, int] = size
        self.screen: pygame.Surface = screen

        self.state: bool = 0

    def get(self):
        return self.state

    def set(self, state: bool):
        self.state = state
        self.update()
        return self.state

    def update(self):
        pygame.draw.rect(self.screen, (255, 255, 255) if self.state else (0,0,0), (self.x * self.size[0] + 10, self.y * self.size[1] + 10, self.size[0], self.size[1]))

    def __repr__(self) -> str:
        return f'Pixel ({self.x}, {self.y}) {self.state}'


pygame.init()

screen = pygame.display.set_mode((WINDOW_WIDTH, WINDOW_HEIGHT))
screen.fill(pygame.Color('LIGHTBLUE'))
pygame.display.set_caption('Art-Net node display simulator')

pixels: list = [pixel(screen, x, y, (PIXEL_SIZE, PIXEL_SIZE)) for y in range(DISP_HEIGHT) for x in range(DISP_WIDHT)]
# pprint.pprint(pixels)


def updateDisplay():
    for pixel in pixels:
        pixel.update()

font = pygame.font.Font(None, 20)
for i, lbl in enumerate(['Menu/Ent', 'Up', 'Down', 'Exit/Back']):
    pygame.draw.rect(screen, (127, 127, 127), (10 + i * 110, DISP_HEIGHT * PIXEL_SIZE + 20, 100, 30))
    screen.blit(font.render(lbl, True, (255, 255, 255)), (10 + i * 110 + 5, DISP_HEIGHT * PIXEL_SIZE + 30))



running = True
while running:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False
    
    updateDisplay()
    pygame.display.update()



    time.sleep(2)
    for i, pixel in enumerate(pixels):
        if i % 2 == 0:
            pixel.set(1)
        else:
            pixel.set(0)

    pygame.display.update()










