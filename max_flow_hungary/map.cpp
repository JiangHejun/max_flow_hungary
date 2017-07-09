#include "map.h"

#define ROUND(d) ((int)((d) + 0.5))
#define IS_HOR(c) (ROUND((c).y) % VER_LEN == 0)
#define IS_VER(c) (ROUND((c).x) % HOR_LEN == 0)

//���·�ߵ�Ԫͨ�ž���
int is_invisible(int lower, int len)
{
	return (lower > UNIT_RADIUS) && (len - lower > UNIT_RADIUS);
}

//�����λ��
void rand_position(struct coord *c)
{
	if(rand_01() < 0.5) { /* Horizontal. ˮƽ�����һ��·��*/
		c->x = rand_range(0, (HOR_INT - 1) * HOR_LEN);
		c->y = rand_range_int(0, VER_INT) * VER_LEN;
	} else {//��ֱ�����һ��·��
		c->x = rand_range_int(0, HOR_INT) * HOR_LEN;
		c->y = rand_range(0, (VER_INT - 1) * VER_LEN);
	}
}

//���������
int rand_dir(struct coord c, int coming_dir)
{
	int	ux = ROUND(c.x) / HOR_LEN,
		uy = ROUND(c.y) / VER_LEN,
		d;
	if(IS_HOR(c) && IS_VER(c)) {//�����·�ڴ�
		if(ux == 0) { /* West bound. ��������·��*/
			d = rand_range_int(0, 2);//���������λ��
			switch(coming_dir) {
			case NORTH:
				if(uy == 0) return EAST;//�������ˮƽ·��
				return d ? NORTH : EAST;
			case EAST://�����������������
				return EAST;
			case SOUTH:
				if(uy == VER_INT - 1) return EAST;//������ϱ�ˮƽ·�Σ�����
				return d ? EAST : SOUTH;
			case WEST:
				if(uy == 0) return SOUTH;//������ϱߣ�ʻ��
				if(uy == VER_INT - 1) return NORTH;//����ڱ��ߣ�ʻ��
				return d ? NORTH : SOUTH;
			default:
				if(uy == 0) return d ? EAST : SOUTH;//������ˮƽ·��
				if(uy == VER_INT - 1) return d ? NORTH : EAST;
				d = rand_range_int(0, 3);
				if(d-- == 0) return NORTH;
				if(d-- == 0) return EAST;
				return SOUTH;
			}
		} else if(ux == HOR_INT - 1) { /* East bound.��������·�� */
			d = rand_range_int(0, 2);
			switch(coming_dir) {
			case NORTH:
				if(uy == 0) return WEST;
				return d ? NORTH : WEST;
			case EAST:
				if(uy == 0) return SOUTH;//������ˮƽλ��
				if(uy == VER_INT - 1) return NORTH;//���������ˮƽλ��
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
		} else if(uy == 0) { /* North bound, without corners.��������·�ڣ�����С������жϹ� */
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
		} else if(uy == VER_INT - 1) { /* South bound, without corners.��������ϱ�·�� */
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
		} else { /* Inner intersections with 4 possible directions. ��û������ϣ�������ĸ�����*/
			if(coming_dir < 0) return rand_range_int(0, 4);
			d = rand_range_int(1, 4);
			return (coming_dir + 2 + d) % 4;
		}
	} else {//���û��·�ڴ�
		if(coming_dir < 0) {
			d = rand_range_int(0, 2);
			if(IS_HOR(c)) {//��ˮƽ����
				if(d == 0) return EAST;
				return WEST;
			} else {//�ڴ�ֱ����
				if(d == 0) return NORTH;
				return SOUTH;
			}
		}
		return coming_dir;
	}
}

//���ҿ�ͨ��·�ߵ�Ԫ�����ر��·�ߵ�Ԫ���
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

//���ؿ�ͨ��·�ߵ�Ԫͨ�ž���
int get_u2v(double x, double y, int uid){
	int ux = uid % HOR_INT,
		uy = uid / VER_INT;
	int distance = sqrt((ux*HOR_LEN-x)*(ux*HOR_LEN-x)+(uy*VER_LEN-y)*(uy*VER_LEN-y));
	return distance;

}

//�����ƶ�
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