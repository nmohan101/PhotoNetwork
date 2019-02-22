# -*- coding: utf-8 -*-
from __future__ import unicode_literals
from django.shortcuts import render
from django.http import HttpResponse
from django_tables2 import RequestConfig
from .models import IncomingData
from .tables import IncomingDataTable


def home(request):
    table = IncomingDataTable(IncomingData.objects.all())
    RequestConfig(request).configure(table)
    return render(request, 'database/home.html', {'table': table})


# Create your views here.
