const l10n = require("../helpers/l10n").default;

export const id = "EVENT_COMETS_RANDOMISE_XYSCROLL";
export const groups = ["Comets"];
export const name = "Comets: Randomise XYScroll";

export const fields = [
	{
		label: "Comets: Randomise XYScroll"
	},
	
  {
    key: "index",
	label: "Index:",
    type: "value",
        width: "50%",
        defaultValue: {
            type: "number",
            value: 0,
        },
  },
];

export const compile = (input, helpers) => {
	const { _callNative, _stackPush, _stackPushConst, _stackPop, variableSetToScriptValue, _declareLocal } = helpers;

    _stackPushConst(0); //Prevents Addition

	const tmpIndex = _declareLocal("tmp_index", 1, true);
    variableSetToScriptValue(tmpIndex, input.index);
    _stackPush(tmpIndex);

	_callNative("comets_randomise_xyscroll");
	
	//End
	_stackPop(2);
};