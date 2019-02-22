import django_tables2 as tables
from .models import IncomingData

class IncomingDataTable(tables.Table):
    class Meta:
        model = IncomingData
        template_name = 'django_tables2/bootstrap.html'
