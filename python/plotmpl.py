# -*- coding: utf-8 -*-

"""
"""

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import Artus.HarryPlotter.plotbase as plotbase


class PlotMpl(plotbase.PlotBase):
	def __init__(self):
		plotbase.PlotBase.__init__(self)
	
	def prepare_args(self, plotData):
		plotbase.PlotBase.prepare_args(self, plotData)
	
	def run(self, plotData):
		plotbase.PlotBase.run(self, plotData)

