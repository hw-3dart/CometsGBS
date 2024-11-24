const l10n = require("../helpers/l10n").default;

export const id = "EVENT_COMETS_GET_DIR";
export const groups = ["Comets"];
export const name = "Comets: Get Player Direction";

export const fields = [
	{
		label: "Comets: Get Player Direction"
	},
  
  {
  type: "group",
  fields: [
	{
		label: "Save in:",
	},
	{
		key: "save_var",
		type: "variable",
		defaultValue: "LAST_VARIABLE",
    },
		],
  },

];

export const compile = (input, helpers) => {
	const { _callNative, _stackPushConst, _stackPop, _setVariable } = helpers;

    _stackPushConst(0); //Prevents Addition

	_callNative("comets_get_dir");
	_setVariable(input.save_var, ".ARG1");
	
	//End
	_stackPop(1);
};