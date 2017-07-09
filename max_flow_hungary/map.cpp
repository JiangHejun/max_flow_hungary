#include "map.h"

#define ROUND(d) ((int)((d) + 0.5))
#define IS_HOR(c) (ROUND((c).y) % VER_LEN == 0)
#define IS_VER(c) (ROUND((c).x) % HOR_LEN == 0)

//检查路边单元通信距离
int is_invisible(int lower, int len)
{
	return (lower > UNIT_RADIUS) && (len - lower > UNIT_RADIUS);
}

//随机化位置
void rand_position(struct coord *c)
{
	if(rand_01() < 0.5) { /* Horizontal. 水平方向的一条路段*/
		c->x = rand_range(0, (HOR_INT - 1) * HOR_LEN);
		c->y = rand_range_int(0, VER_INT) * VER_LEN;
	} else {//垂直方向的一条路段
		c->x = rand_range_int(0, HOR_INT) * HOR_LEN;
		c->y = rand_range(0, (VER_INT - 1) * VER_LEN);
	}
}

//随机化方向
int rand_dir(struct coord c, int coming_dir)
{
	int	ux = ROUND(c.x) / HOR_LEN,
		uy = ROUND(c.y) / VER_LEN,
		d;
	if(IS_HOR(c) && IS_VER(c)) {//如果在路口处
		if(ux == 0) { /* West bound. 在最西边路口*/
			d = rand_range_int(0, 2);//随机化两个位置
			switch(coming_dir) {
			case NORTH:
				if(uy == 0) return EAST;//如果北边水平路段
				return d ? NORTH : EAST;
			case EAST://如果朝东，继续往东
				return EAST;
			case SOUTH:
				if(uy == VER_INT - 1) return EAST;//如果在南边水平路段，往东
				return d ? EAST : SOUTH;
			case WEST:
				if(uy == 0) return SOUTH;//如果在南边，驶出
				if(uy == VER_INT - 1) return NORTH;//如果在北边，驶出
				return d ? NORTH : SOUTH;
			default:
				if(uy == 0) return d ? EAST : SOUTH;//如果在最北水平路段
				if(uy == VER_INT - 1) return d ? NORTH : EAST;
				d = rand_range_int(0, 3);
				if(d-- == 0) return NORTH;
				if(d-- == 0) return EAST;
				return SOUTH;
			}
		} else if(ux == HOR_INT - 1) { /* East bound.如果在最东边路口 */
			d = rand_range_int(0, 2);
			switch(coming_dir) {
			case NORTH:
				if(uy == 0) return WEST;
				return d ? NORTH : WEST;
			case EAST:
				if(uy == 0) return SOUTH;//如果在最北水平位置
				if(uy == VER_INT - 1) return NORTH;//如果在最南水平位置
				return d ? NORTH : SOUTH;
			case SOUTH:
				if(uy == VER_INT - 1) return WEST;
				return d ? SOUTH : WEST;
			case WEST:
				return WEST;
			default:
				if(uy == 0) return d ? SOUTH : WEST;
				if(uy == VER_INT - 1) return d ? NORTH : WEST;
				d = rand_range_int(0, 3);
				if(d-- == 0) return NORTH;
				if(d-- == 0) return SOUTH;
				return WEST;
			}
		} else if(uy == 0) { /* North bound, without corners.如果在最北边路口，上述小情况已判断过 */
			d = rand_range_int(0, 2);
			switch(coming_dir) {
			case NORTH: return d ? EAST : WEST;
			case EAST: return d ? EAST : SOUTH;
			case SOUTH: return SOUTH;
			case WEST: return d ? SOUTH : WEST;
			default: d = rand_range_int(0, 3);
				if(d-- == 0) return EAST;
				if(d-- == 0) return SOUTH;
				return WEST;
			}
		} else if(uy == VER_INT - 1) { /* South bound, without corners.如果在最南边路口 */
			d = rand_range_int(0, 2);
			switch(coming_dir) {
			case NORTH: return NORTH;
			case EAST: return d ? EAST : NORTH;
			case SOUTH: return d ? EAST : WEST;
			case WEST: return d ? NORTH : WEST;
			default: d = rand_range_int(0, 3);
				if(d-- == 0) return NORTH;
				if(d-- == 0) return EAST;
				return WEST;
			}
		} else { /* Inner intersections with 4 possible directions. 都没在最边上，随机化四个方向*/
			if(coming_dir < 0) return rand_range_int(0, 4);
			d = rand_range_int(1, 4);
			return (coming_dir + 2 + d) % 4;
		}
	} else {//如果没在路口处
		if(coming_dir < 0) {
			d = rand_range_int(0, 2);
			if(IS_HOR(c)) {//在水平方向
				if(d == 0) return EAST;
				return WEST;
			} else {//在垂直方向
				if(d == 0) return NORTH;
				return SOUTH;
			}
		}
		return coming_dir;
	}
}

//查找可通信路边单元，返回编号路边单元编号
int find_unit(int x, int y)
{
	int ux = x / HOR_LEN,
		uy = y / VER_LEN,
		dx = x % HOR_LEN,
		dy = y % VER_LEN;
	if(is_invisible(dx, HOR_LEN) || is_invisible(dy, VER_LEN)) return -1;
	if(dx > UNIT_RADIUS) ux++;
	if(dy > UNIT_RADIUS) uy++;
	return ux + uy * HOR_INT;
}

//返回可通信路边单元通信距离
int get_u2v(double x, double y, int uid){
	int ux = uid % HOR_INT,
		uy = uid / VER_INT;
	int distance = sqrt((ux*HOR_LEN-x)*(ux*HOR_LEN-x)+(uy*VER_LEN-y)*(uy*VER_LEN-y));
	return distance;

}

//车辆移动
int move(struct coord cur, int dir, struct coord *next, double speed)
{
	double dist;
	int ux = ROUND(cur.x) / HOR_LEN,
		uy = ROUND(cur.y) / VER_LEN,
		new_dir;

	next->x = cur.x;
	next->y = cur.y;	
	
	switch(dir) {

	case NORTH:
		dist = cur.y - uy * VER_LEN;
		if(dist > speed) {
			next->y -= speed;
			return NORTH;
		}
		next->y = uy * VER_LEN;
		new_dir = rand_dir(*next, NORTH);
		if(new_dir == EAST) next->x += speed - dist;
		else if(new_dir == WEST) next->x -= speed - dist;
		else next->y -= speed - dist;
		return new_dir;

	case EAST:
		if(ux == HOR_INT - 1) {
			new_dir = rand_dir(*next, EAST);
			if(new_dir == NORTH) next->y -= speed;
			else next->y += speed;
			return new_dir;
		} else {
			dist = (ux + 1) * HOR_LEN - cur.x;
			if(dist > speed) {
				next->x += speed;
				return EAST;
			}
			next->x = (ux + 1) * HOR_LEN;
			new_dir = rand_dir(*next, EAST);
			if(new_dir == NORTH) next->y -= speed - dist;
			else if(new_dir == EAST) next->x = cur.x + speed;
			else next->y += speed - dist;
			return new_dir;
		}

	case SOUTH:
		if(uy == VER_INT - 1) {
			new_dir = rand_dir(*next, SOUTH);
			if(new_dir == EAST) next->x += speed;
			else next->x -= speed;
			return new_dir;
		} else {
			dist = (uy + 1) * VER_LEN - cur.y;
			if(dist > speed) {
				next->y += speed;
				return SOUTH;
			}
			next->y = (uy + 1) * VER_LEN;
			new_dir = rand_dir(*next, SOUTH);
			if(new_dir == EAST) next->x += speed - dist;
			else if(new_dir == SOUTH) next->y = cur.y + speed;
			else next->x -= speed - dist;
			return new_dir;
		}

	case WEST:
		dist = cur.x - ux * HOR_LEN;
		if(dist > speed) {
			next->x -= speed;
			return WEST;
		} else {
			next->x = ux * HOR_LEN;
			new_dir = rand_dir(*next, WEST);
			if(new_dir == NORTH) next->y -= speed - dist;
			else if(new_dir == SOUTH) next->y += speed - dist;
			else next->x = cur.x - speed;
			return new_dir;
		}

	default: return move(cur, rand_dir(cur, -1), next, speed);

	}

	return -1;
}