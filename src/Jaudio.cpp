#include "z12.hpp"


struct z12 : Module {
	enum ParamIds {
		PROB_1,
		PROB_2,
		PROB_3,
		NUM_PARAMS
	};
	enum InputIds {
		PROB_1_INPUT,
		PROB_2_INPUT,
		PROB_3_INPUT,
		CLOCK_INPUT,
		NUM_INPUTS
	};
	enum OutputIds {
		PROB_1_OUTPUT,
		PROB_2_OUTPUT,
		PROB_3_OUTPUT,
		NUM_OUTPUTS
	};
	enum LightIds {
		ON_LIGHT,
		NUM_LIGHTS
	};

	float phase = 0.0;
	float blinkPhase = 0.0;

	z12() : Module(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS) {}
	void step() override;

	// For more advanced Module features, read Rack's engine.hpp header file
	// - toJson, fromJson: serialization of internal data
	// - onSampleRateChange: event triggered by a change of sample rate
	// - onReset, onRandomize, onCreate, onDelete: implements special behavior when user clicks these from the context menu
};


void z12::step() {
	// Implement a simple sine oscillator
	// float deltaTime = engineGetSampleTime();

	// Compute the sine output
	outputs[PROB_1_OUTPUT].value = inputs[PROB_1_INPUT].value;
	outputs[PROB_2_OUTPUT].value = inputs[PROB_2_INPUT].value;
	outputs[PROB_3_OUTPUT].value = inputs[PROB_3_INPUT].value;

struct z12Widget : ModuleWidget {
	z12Widget(z12 *module);
};

z12Widget::z12Widget(z12 *module) : ModuleWidget(module) {
	setPanel(SVG::load(assetPlugin(plugin, "res/z12.svg")));

	// addParam(ParamWidget::create<Davies1900hBlackKnob>(Vec(28, 87), module, MyModule::PITCH_PARAM, -3.0, 3.0, 0.0));

	addInput(Port::create<PJ301MPort>(Vec(0, 186), Port::INPUT, module, z12::PROB_1_INPUT));
	addInput(Port::create<PJ301MPort>(Vec(33, 186), Port::INPUT, module, z12::PROB_2_INPUT));
	addInput(Port::create<PJ301MPort>(Vec(66, 186), Port::INPUT, module, z12::PROB_3_INPUT));

	addOutput(Port::create<PJ301MPort>(Vec(0, 275), Port::OUTPUT, module, z12::PROB_1_OUTPUT));
	addOutput(Port::create<PJ301MPort>(Vec(33, 275), Port::OUTPUT, module, z12::PROB_2_OUTPUT));
	addOutput(Port::create<PJ301MPort>(Vec(66, 275), Port::OUTPUT, module, z12::PROB_3_OUTPUT));
}

// Specify the Module and ModuleWidget subclass, human-readable
// author name for categorization per plugin, module slug (should never
// change), human-readable module name, and any number of tags
// (found in `include/tags.hpp`) separated by commas.
Model *modelz12 = Model::create<z12, z12Widget>("Jaudio", "Jaudio", "z12", SEQUENCER_TAG);
