-- premake util functions

function _dump(a)
	local function dump(o)
	   if type(o) == 'table' then
		  local s = '{ '
		  for k,v in pairs(o) do
			 if type(k) ~= 'number' then k = '"'..k..'"' end
			 s = s .. '['..k..'] = ' .. dump(v) .. ','
		  end
		  return s .. '} '
	   else
		  return tostring(o)
	   end
	end
	local result = dump(a)
	print("Dump: ", result)
end

function _flatten(arr)
	local result = { }
		
	local function flatten(arr)
		for k, v in pairs(arr) do
			if type(v) == "table" then
				flatten(v)
			else
				table.insert(result, v)
			end
		end
	end
		
	flatten(arr)
	--_dump(result)
	return result
end
