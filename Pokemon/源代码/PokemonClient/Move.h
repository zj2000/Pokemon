#ifndef MOVE_H
#define MOVE_H

#include <iostream>
#include <string>

enum class Type { water, fire, grass, electric, normal };

class Move //��ʽ
{
	int id;//���
	int accuracy;//��ʽ�����У����������ʣ�
	int power;//��ʽ������

	Type moveType;//��ʽ�����ԣ���ʽ����������С����һ�£�����������˺�
	bool isCriticalHit; //�Ƿ������ײ�����������ʽ
	bool isPhysical;//�Ƿ�����������true����������false�����⹥��

	std::string moveName;//��ʽ����

public:
	Move(int id, int accuracy, int power, bool isCri, bool isPhy, std::string type, std::string name);
	int GetAccuracy() const;//������ʽ����
	Type GetType() const;//������ʽ������
	int GetPower() const;//������ʽ������
	bool GetIsPhysical()const;//������ʽ�Ĺ�������
	std::string GetName() const;//������ʽ��
	bool GetIsCritical() const;
};


#endif // !MOVE_H

