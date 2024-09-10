#!/usr/bin/env python3

import pygame
from pygame.locals import *

# Initialize Pygame
pygame.init()

# Constants
WIDTH = 128
HEIGHT = 64
GRID_SIZE = 8
WINDOW_WIDTH = WIDTH * GRID_SIZE
WINDOW_HEIGHT = HEIGHT * GRID_SIZE + 50

# Colors
WHITE = (255, 255, 255)
BLACK = (0, 0, 0)
BUTTON_COLOR = (170, 170, 170)
BUTTON_TEXT_COLOR = (0, 0, 0)

# Create the window
screen = pygame.display.set_mode((WINDOW_WIDTH, WINDOW_HEIGHT))
pygame.display.set_caption('Pixel Grid with Buttons')

# Create a pixel array to simulate the display
pixels = [[BLACK for _ in range(WIDTH)] for _ in range(HEIGHT)]

# Function to draw the grid
def draw_grid():
    for y in range(HEIGHT):
        for x in range(WIDTH):
            color = WHITE if pixels[y][x] == 1 else BLACK
            pygame.draw.rect(screen, color, (x * GRID_SIZE, y * GRID_SIZE, GRID_SIZE, GRID_SIZE))

# Function to draw the buttons
def draw_buttons():
    button_width = 100
    button_height = 30
    space = 10
    
    # Button positions
    button1_pos = (10, HEIGHT * GRID_SIZE + 10)
    button2_pos = (button1_pos[0] + button_width + space, button1_pos[1])
    button3_pos = (button2_pos[0] + button_width + space, button2_pos[1])
    button4_pos = (button3_pos[0] + button_width + space, button3_pos[1])
    
    # Draw buttons
    pygame.draw.rect(screen, BUTTON_COLOR, (button1_pos[0], button1_pos[1], button_width, button_height))
    pygame.draw.rect(screen, BUTTON_COLOR, (button2_pos[0], button2_pos[1], button_width, button_height))
    pygame.draw.rect(screen, BUTTON_COLOR, (button3_pos[0], button3_pos[1], button_width, button_height))
    pygame.draw.rect(screen, BUTTON_COLOR, (button4_pos[0], button4_pos[1], button_width, button_height))
    
    # Add text to buttons
    font = pygame.font.Font(None, 20)
    text1 = font.render("Menu/Ent", True, BUTTON_TEXT_COLOR)
    text2 = font.render("Up", True, BUTTON_TEXT_COLOR)
    text3 = font.render("Down", True, BUTTON_TEXT_COLOR)
    text4 = font.render("Exit/Back", True, BUTTON_TEXT_COLOR)
    
    screen.blit(text1, (button1_pos[0] + 5, button1_pos[1] + 5))
    screen.blit(text2, (button2_pos[0] + 5, button2_pos[1] + 5))
    screen.blit(text3, (button3_pos[0] + 5, button3_pos[1] + 5))
    screen.blit(text4, (button4_pos[0] + 5, button4_pos[1] + 5))

# Main loop
running = True
while running:
    for event in pygame.event.get():
        if event.type == QUIT:
            running = False
    
    # Draw the grid and buttons
    draw_grid()
    draw_buttons()
    
    # Update the display
    pygame.display.update()

# Quit Pygame
pygame.quit()