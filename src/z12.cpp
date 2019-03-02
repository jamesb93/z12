#include "JamesBradbury.hpp"
#include "dsp/digital.hpp"
#include <limits>

struct z12 : Module {
	enum ParamIds {
		ENUMS(PROB, 8),
		NUM_PARAMS
	};
	enum InputIds {
		ENUMS(PROB_INPUT, 8),
		EXT_CLOCK_INPUT,
		NUM_INPUTS
	};
	enum OutputIds {
		ENUMS(PROB_OUTPUT, 8),
		NUM_OUTPUTS
	};
	enum LightIds {
		ON_LIGHT,
		NUM_LIGHTS
	};

	SchmittTrigger clockTrigger;
	PulseGenerator pulseGenerators[8];
	float probArray[8] = {};
	float overflowArray[8] = {};
	bool change;
	float probSum;
	int switchOn;
	double maxOverflow = -std::numeric_limits<double>::infinity();

	// Interface Arrays


	z12() : Module(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS) {}
	void step() override;
};

void z12::step() {
	if (inputs[EXT_CLOCK_INPUT].active) {
		if (clockTrigger.process(inputs[EXT_CLOCK_INPUT].value)) {

			// These have to be here! Always reset the probSum and maxOverflow
			maxOverflow = -std::numeric_limits<double>::infinity();
			probSum = 0.0;

			for (int i = 0; i < 8; i++) {
				// If the probability inlet is connected
				if (inputs[PROB_INPUT + i].active) {
						probArray[i] = inputs[PROB_INPUT + i].value;
				}

				// Otherwise defer to the value of the knob
				else {
						probArray[i] = params[PROB + i].value;
				}
				// calculate the probsum as values are stored in probArray
				probSum += probArray[i];
			}

			// // Calculate probsum regardless of what inputs are connected
			// for (int i = 0; i < 8; i++) {
			// 	probSum += probArray[i];
			// }

			double normFactor = 1.0 / probSum;

			// Stuff the params into an array mult by normFactor
			for (int i = 0; i < 8; i++) {
				probArray[i] *= normFactor;
			}

			// Find Max index while adding the overflowArray + probArray
			for (int i = 0; i < 8; i++) {
				overflowArray[i] += probArray[i];
			}

			for (int i = 0; i < 8; i++) {
				if (overflowArray[i] > maxOverflow) {
				maxOverflow = overflowArray[i];
				switchOn = i;
				}
			}

			// Trigger the corresponding pulseGenerator (each output has its own)
			for (int i = 0; i < 8; i++) {
				if (i == switchOn) {
					pulseGenerators[i].trigger(1e-3f);
				}
			}
			overflowArray[switchOn] -= 1.0;
		}

		for (int i = 0; i < 8; i++) {
			outputs[PROB_OUTPUT + i].value = pulseGenerators[i].process(engineGetSampleTime()) ? 10.0f : 0.0f;
		}
	}
}

struct z12Widget : ModuleWidget {
	z12Widget(z12 *module);
};

z12Widget::z12Widget(z12 *module) : ModuleWidget(module) {
	setPanel(SVG::load(assetPlugin(plugin, "res/z12_done.svg")));

	// measurements in mm for ports
	float panelWidth = 96.52f;
	float margin = 5.0f;
	float portWidth = 8.4666f;
	float portsArea = panelWidth - (margin * 2.0f);
	float horizontalStep = (portsArea - portWidth) / 7.0f;
	float offset;

	static const float knobPosX[8] = {4.053, 15.203, 26.354, 37.504, 48.655, 59.805, 70.956, 82.106};

	addChild(Widget::create<ScrewSilver>(Vec(15, 0)));
	addChild(Widget::create<ScrewSilver>(Vec(box.size.x - 30, 0)));
	addChild(Widget::create<ScrewSilver>(Vec(15, 365)));
	addChild(Widget::create<ScrewSilver>(Vec(box.size.x - 30, 365)));

	for (int i = 0; i < 8; i++) {
		offset = mm2px(margin + (horizontalStep * i));
		addInput(Port::create<PJ301MPort>(Vec(offset, 100), Port::INPUT, module, z12::PROB + i));
	}

	// Knobs
	for (int i = 0; i < 8; i++) {
		offset = mm2px(knobPosX[i]);
		addParam(ParamWidget::create<RoundBlackKnob>(Vec(offset, 200), module, z12::PROB + i, 0.0f, 1.0f, 0.5f));
	}

	// z12 gate trigger outputs
	for (int i = 0; i < 8; i++) {
		offset = mm2px(margin + (horizontalStep * i));
		addOutput(Port::create<PJ301MPort>(Vec(offset, 300), Port::OUTPUT, module, z12::PROB_OUTPUT + i));
	}

	// External Clock Input
	addInput(Port::create<PJ301MPort>(Vec(20, 50), Port::INPUT, module, z12::EXT_CLOCK_INPUT));
}

Model *modelz12 = Model::create<z12, z12Widget>("James Bradbury", "James Bradbury", "z12 Stochastic Sequencer", SEQUENCER_TAG);
