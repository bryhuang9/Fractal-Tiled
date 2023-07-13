#include "EasyBMP.h"

#include <iostream>
#include <vector>
#include <cmath>
#include <memory>
#include <future>
#include <thread>
#include <chrono>
#include <unistd.h>
#include <memory>

#include "Fractal.h"
#include "FractalMessage.h"
#include "ThreadSafeQueue.h"

typedef ThreadSafeQueue<std::shared_ptr<FractalMessage>> FractalMessageQueue;
typedef std::vector<std::shared_ptr<std::thread>> ThreadVector;

using namespace std;

void render_fractal(std::shared_ptr<FractalMessage> msg) {
    fractal(msg->get_output_image(), msg->get_left(), msg->get_top(), msg->get_x_size(), msg->get_y_size(),
        msg->get_min_x(), msg->get_min_y(), msg->get_max_x(), msg->get_max_y(),
        msg->get_image_x(), msg->get_image_y());
};

void do_tile(EasyBMP::Image * img, float left, float top, float xsize, float ysize, float img_min_x, float img_min_y, float img_max_x, float img_max_y, float img_x_size, float img_y_size) {

    // make a message, and render it directly!

    std::shared_ptr<FractalMessage> msg = std::make_shared<FractalMessage>(img, left, top, xsize, ysize, img_min_x, img_min_y, img_max_x, img_max_y, img_x_size, img_y_size);
    render_fractal(msg);
};

void consumer_func(FractalMessageQueue* queue) {
    while (true) {
        std::shared_ptr<FractalMessage> msg = queue->pop();

        if (msg == nullptr) {
            return;
        }

        render_fractal(msg);
    }
}

int main(){
    // Define an RGB color for black 
    EasyBMP::RGBColor black(0, 0, 0);  

    // make an image initialized with black .  Image size is 512x512
    // Image size should always be POT (power of 2)
    size_t img_size_x = 512;
    size_t img_size_y = 512;

    EasyBMP::Image img(img_size_x, img_size_y, "output.bmp", black);

    // the number of tiles in X & Y 
    size_t num_tiles_x = 4;
    size_t num_tiles_y = 4;

    // this is the size of the tiles in the output image
    size_t img_tile_size_x = img_size_x / num_tiles_x;
    size_t img_tile_size_y = img_size_y / num_tiles_y; 

    // the overall area we are rendering in "fractal space"    
    float left = -1.75; float top = -0.25;
    float xsize = 0.25; float ysize = 0.45;

    // how big each tile is in x,y in "fractal space"
    float fractal_tile_size_x = xsize / num_tiles_x;
    float fractal_tile_size_y = ysize / num_tiles_y; 

    FractalMessageQueue queue;
    ThreadVector threads;

    for (size_t i = 0; i < 8; i++) {
        std::shared_ptr<std::thread> thread = std::make_shared<std::thread>(consumer_func, &queue);
        threads.push_back(thread);
    }

    for (size_t i = 0; i < num_tiles_x; i++) {
	for (size_t j = 0; j < num_tiles_y; j++) {
		// calculate the coords for the output tile, in bitmap(img) space 
		size_t img_tile_min_x = 0 + (img_tile_size_x * i);
		size_t img_tile_min_y = 0 + (img_tile_size_y * j);

		size_t img_tile_max_x = img_tile_min_x + img_tile_size_x;
		size_t img_tile_max_y = img_tile_min_y + img_tile_size_y;

		// calculate the coords for the output tile in fractal space
	        float fractal_tile_min_x = left + (i * fractal_tile_size_x);
	        float fractal_tile_min_y = top + (j * fractal_tile_size_y);	

		float fractal_tile_max_x = fractal_tile_min_x + fractal_tile_size_x;
	        float fractal_tile_max_y = fractal_tile_min_y + fractal_tile_size_y;	

		cout << "TILE: " << i << "," << j <<  " img_tile_min_x : " << img_tile_min_x << " img_tile_min_y : " << img_tile_min_y  \
		     << " img_tile_max_x : " << img_tile_max_x << " img_tile_max_y : " << img_tile_max_y << endl;

		cout << "FRACTAL_TILE: " << i << "," << j << " fractal_tile_min_x: " << fractal_tile_min_x << ", fractal_tile_min_y: "<< fractal_tile_min_y  \
	             << ", fractal_tile_max_x: " << fractal_tile_max_x << ", fractal_tile_max_y: " << fractal_tile_max_y << endl; 

		std::future<void> fr = async(do_tile, &img, fractal_tile_min_x, fractal_tile_min_y, fractal_tile_max_x, fractal_tile_max_y, img_tile_min_x, img_tile_min_y, img_tile_max_x, img_tile_max_y, img_size_x, img_size_y); 
		
	}
    }
    
    queue.abort();

    for (auto thread : threads) {
        thread->join();
    }

    //do_tile(&img, left, top, xsize, ysize, 0, 0, output_bitmap_size_x, output_bitmap_size_y, output_bitmap_size_x, output_bitmap_size_y);

    img.Write();
    return 0;
}
