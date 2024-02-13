-- #class: [static] Класс содержит различные методы для алгебраических рассчетов.
Algebra = 
{
	VectorToNumVector = function (v)
		vector = {}
		if (v.x == nil) then vector[1] = v[1] else vector[1] = v.x end
		if (v.y == nil) then vector[2] = v[2] else vector[2] = v.y end
		if (v.z == nil) then vector[3] = v[3] else vector[3] = v.z end
		return vector
	end,

	VectorToChrVector = function (v)
		vector = {}
		if (v.x == nil) then vector.x = v[1] else vector.x = v.x end
		if (v.y == nil) then vector.y = v[2] else vector.y = v.y end
		if (v.z == nil) then vector.z = v[3] else vector.z = v.z end
		return vector
		
	end,

	-- Вычесть вектор v2 из вектора v1 поэлементно.
	VectorSubVector = function (v1, v2)
		v1 = Algebra.VectorToNumVector(v1)
		v2 = Algebra.VectorToNumVector(v2)
		return {v1[1] - v2[1], v1[2] - v2[2], v1[3] - v2[3]}
	end,

	-- Прибавить вектор v2 к вектору v1 поэлементно.
	VectorAddVector = function (v1, v2)
		v1 = Algebra.VectorToNumVector(v1)
		v2 = Algebra.VectorToNumVector(v2)
		return {v1[1] + v2[1], v1[2] + v2[2], v1[3] + v2[3]}
	end,

	-- Разделить вектор v1 на вектор v2 поэлементно.
	VectorDivVector = function (v1, v2)
		v1 = Algebra.VectorToNumVector(v1)
		v2 = Algebra.VectorToNumVector(v2)
		local result = {1, 1, 1}
		if (v2[1] == 0) then result[1] = 0 else result[1] = v1[1] / v2[1] end
		if (v2[2] == 0) then result[2] = 0 else result[2] = v1[2] / v2[2] end
		if (v2[3] == 0) then result[3] = 0 else result[3] = v1[3] / v2[3] end
		return result
	end,
	
	VectorMultNumber = function (v, num)
		local vector = {}
		for k,v in pairs(v) do
			vector[k] = v*num
		end
		return vector
	end,	
		
	MatrixMultVector = function (M, V)
		local result = {0, 0, 0}
		result[1] = M[1][1] * V[1] + M[1][2] * V[2] + M[1][3] * V[3]
		result[2] = M[2][1] * V[1] + M[2][2] * V[2] + M[2][3] * V[3]
		result[3] = M[3][1] * V[1] + M[3][2] * V[2] + M[3][3] * V[3]
		return result
	end,
	
	VectorMultMatrix = function (V, M)
		local result = {0, 0, 0}
		result[1] = M[1][1] * V[1] + M[2][1] * V[2] + M[3][1] * V[3]
		result[2] = M[1][2] * V[1] + M[2][2] * V[2] + M[3][2] * V[3]
		result[3] = M[1][3] * V[1] + M[2][3] * V[2] + M[3][3] * V[3]
		return result
	end,
	
	-- #meth: Получить средне значение числа между y1 и y2.
	-- #x1: значение начала области определения.
	-- #x2: значение конца области определения.
	-- #y1: значение начала области значений.
	-- #y2: значение конца области значений.
	-- #x0: значение из (x1, x2) для которого нужно найти отображение в (y1, y2).
	-- #returns: искомое отображение x в Y.
	GetMidValueN = function(x1, x2, y1, y2, x0)
		local k = (y2 - y1) / (x2 - x1)
		return (x0 - x1)*k + y1
	end,
	
	-- #meth: Получить средне значение n-мерного вектора между v1 и v2.
	-- #x1: значение начала области определения.
	-- #x2: значение конца области определения.
	-- #y1: вектор начала области значений.
	-- #y2: вектор конца области значений.
	-- #x0: значение из (x1, x2) для которого нужно найти отображение в (v1, v2).
	-- #returns: искомый вектор.
	GetMidValueV = function(x1, x2, v1, v2, x0)
		local result = {}
		for i = 1, #v1 do
			result[i] = Algebra.GetMidValueN(x1, x2, v1[i], v2[i], x0)
		end
		return result
	end,
	
	-- Возвращает резуольтат умножения матриц M1*M2.
	MatrixMultMatrix = function (M1, M2)
		local result = {}
		for i =1,#M1 do
			for j =1,#M2 do
				local sum = 0
				for k = 1,#M2 do
					sum = sum + M1[i][k]*M2[k][j]
				end
				result[i][j] = sum
			end
		end
		
		return result
	end	
}