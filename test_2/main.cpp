#include <queue>
using namespace std;
template <int Frame_len, int Frame_queue, int Queue_len>
class Fifo
{
	/*≈сли выдел€ть под каждый пакет фиксированный размер структуры то вспомагательна€ queue будет не нужна,
	можно развернуть еЄ в массив структур, будет класс без использовани€ <queue> повтор€ющий все методы и пол€ содержащиес€ в Fifo работающие с буффером buf[].
	“ак же можно создать массив структур в котором динамически будет выдел€тьс€ место в пам€ти под char[] data,
	но придетс€ дл€ подстчета общего размера буффера проходить по всем структурам и суммировать общее использованное пространство */

	struct str //структура хранени€ packetId, dataCount и указател€ на начало данных в buf[]
	{
		unsigned int id, len;
		unsigned char* data;
		str(unsigned char* ptr, unsigned int dataCount, unsigned int packetId)
		{
			id = packetId;
			len = dataCount;
			data = ptr;
		}
	};
	queue<str> q; 
	unsigned char buf[Queue_len]; //кольцевой буффер
	unsigned char* st = buf; //указатель на начало
	unsigned char* en = buf; //указатель на конец

	bool mem_check(unsigned int count) //метод проверки буффера на количество свободных €чеек
	{
		if (en >= st)
		{
			if (en - st + count <= Queue_len) return true;
			else return false;
		}
		else
		{
			if (st - en + count <= Queue_len) return true;
			else return false;
		}
	} 
	void check_empty(void)
	{
		if (en == st)
		{
			st = buf;
			en = buf;
		}
	}
	void en_inc(void) // инкремент указател€ конца
	{
		if (en < &buf[Queue_len - 1]) en++;
		else en = buf;
		check_empty();
	}
	void st_inc(void) // инкремент указател€ начала
	{
		if (st < &buf[Queue_len - 1]) st++;
		else st = buf;
		check_empty();
	}
	void copy_in(unsigned char* data, unsigned int count) //метод копировани€ в buf[]
	{
		for (int i = 0; i < count; i++)
		{
			*(en) = *(data + i);
			en_inc();
		}
	}
	void copy_out(unsigned char* buffer, unsigned int bufferSize, unsigned int count) //метод копировани€ из buf[]
	{
		for (int i = 0; i < count; i++)
		{
			if (i < bufferSize) //вот тут спорный момент, в задании указано возвращать все, даже если пакет превышает размер буфера, но писать в непон€тные области пам€ти неочень. ≈сли все таки очень, то нужно убрать эту строку.
				*(buffer + i) = *(st); 
			*(st) = 0;
			st_inc();
		}
	}
public:

	bool Push(unsigned char* data, unsigned int dataCount, unsigned int packetId) 
	{
		if (mem_check(dataCount) && dataCount <= Frame_len && q.size() <= Frame_queue) //проверка на услови€
		{
			str tmp(en, dataCount, packetId);
			q.push(tmp);
			copy_in(data, dataCount);
			return true;
		}
		else return false;

	}
	bool Pop(unsigned char* buffer, unsigned int bufferSize, unsigned int& dataCount, unsigned int& packetId)
	{
		if (q.size() != 0)
		{
			str tmp = q.front();
			dataCount = tmp.len;
			packetId = tmp.id;
			copy_out(buffer, bufferSize, tmp.len);
			q.pop();
			return true;
		}
		else return false;
	}
};


int main(void)
{
	unsigned char data[10] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j' };
	struct str
	{
		unsigned char ret[10];
		unsigned int id, len;
	};
	str st[4];
	Fifo<10, 15, 35> ff;
	while (true)
	{
		ff.Push(data, 10, 99); //поставьте брейкпоинты на все строки ниже дл€ просмотра логики работы, в дебагере смотрите на st и ff.buf
		ff.Push(data, 10, 99);
		ff.Push(data, 10, 99);
		ff.Pop(st[0].ret, sizeof(st[0].ret), st[0].id, st[0].len);
		ff.Push(data, 10, 99);
		int i = 1; //кроме этой)
		while (ff.Pop(st[i].ret, sizeof(st[i].ret), st[i].id, st[i].len)) i++;
		ff.Push(data, 10, 99);
		ff.Push(data, 10, 99);
		ff.Push(data, 10, 99);
	}
	return 0;
}