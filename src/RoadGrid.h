#ifndef STROG_ROADGRID_H
#define STROG_ROADGRID_H


class RoadGrid
{
public:
	std::vector<uint8_t> grid;
	int width{};
	int height{};
	int mid_y_coord{}; // for the horizonal road's y coordinate
	float min_x_coord{};
	float min_y_coord{};
	float grid_to_map_offset_x{}; // this determines the number of pixels along the x-axis from the map the grid starts
	float grid_to_map_offset_y{}; // this determines the number of pixels along the y-axis from the map the grid starts


	void send_entity_to_nearest_non_0_ref(
		EntityCircle* entity, 
		const float offset_x, 
		const float offset_y,
		const float texture_width
	) const
	{
		sf::Vector2f entity_pos{ entity->get_shape().getPosition() };
		int grid_ref_x{ static_cast<int>((entity_pos.x - offset_x) / texture_width) };
		grid_ref_x = grid_ref_x >= 0 ? grid_ref_x : 0;
		int grid_ref_y{ static_cast<int>((entity_pos.y - offset_y) / texture_width) };
		grid_ref_y = grid_ref_y >= 0 ? grid_ref_y : 0;
		int current_grid_value{ grid[(grid_ref_y * width) + grid_ref_x] };

		int positive_vertical_iterator{ 1 };
		int negative_vertical_iterator{ -1 };
		int positive_horizontal_iterator{ 1 };
		int negative_horizontal_iterator{ -1 };
		while (!current_grid_value)
		{
			if ((grid_ref_y + positive_vertical_iterator) < height)
			{
				current_grid_value = grid[((grid_ref_y + positive_vertical_iterator) * width) + grid_ref_x];
				if (current_grid_value)
				{
					grid_ref_y += positive_vertical_iterator;
					break;
				}
				++positive_vertical_iterator;
			}
			if ((grid_ref_y + negative_vertical_iterator) >= 0)
			{
				current_grid_value = grid[((grid_ref_y + negative_vertical_iterator) * width) + grid_ref_x];
				if (current_grid_value)
				{
					grid_ref_y += negative_vertical_iterator;
					break;
				}
				--negative_vertical_iterator;
			}

			if ((grid_ref_x + positive_horizontal_iterator) < width)
			{
				current_grid_value = grid[(grid_ref_y * width) + (grid_ref_x + positive_horizontal_iterator)];
				if (current_grid_value)
				{
					grid_ref_x += positive_horizontal_iterator;
					break;
				}
				++positive_horizontal_iterator;
			}
			if ((grid_ref_x + negative_horizontal_iterator) >= 0)
			{
				current_grid_value = grid[(grid_ref_y * width) + (grid_ref_x + negative_horizontal_iterator)];
				if (current_grid_value)
				{
					grid_ref_x += negative_horizontal_iterator;
					break;
				}
				--negative_horizontal_iterator;
			}
		}
		entity->set_position(offset_x + (grid_ref_x * texture_width), offset_y + (grid_ref_y * texture_width));
	}
};

#endif // !STROG_ROADGRID_H
