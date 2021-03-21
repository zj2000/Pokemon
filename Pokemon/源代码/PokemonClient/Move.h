#ifndef MOVE_H
#define MOVE_H

#include <iostream>
#include <string>

enum class Type { water, fire, grass, electric, normal };

class Move //招式
{
	int id;//标号
	int accuracy;//招式的命中（不是命中率）
	int power;//招式的威力

	Type moveType;//招式的属性，招式的属性若与小精灵一致，则可以增大伤害
	bool isCriticalHit; //是否是容易产生暴击的招式
	bool isPhysical;//是否是物理攻击，true是物理攻击，false是特殊攻击

	std::string moveName;//招式名字

public:
	Move(int id, int accuracy, int power, bool isCri, bool isPhy, std::string type, std::string name);
	int GetAccuracy() const;//返回招式的命
	Type GetType() const;//返回招式的属性
	int GetPower() const;//返回招式的威力
	bool GetIsPhysical()const;//返回招式的攻击类型
	std::string GetName() const;//返回招式名
	bool GetIsCritical() const;
};


#endif // !MOVE_H

